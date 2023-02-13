// File VRP_Helpers.cc
#include "VRP_Helpers.hh"

// constructor
VRP_StateManager::VRP_StateManager(const VRP_Input & pin) 
  : StateManager<VRP_Input,VRP_State>(pin, "VRP_StateManager")  {} 

// initial state builder 
void VRP_StateManager::RandomState(VRP_State& st) 
{
  // Insert the code that creates a random state in object st
  st.ResetState();
  unsigned id_cliente, id_truck;

  while(!st.TuttiServiti())
  {
    do
    {
      id_truck = Random::Uniform<unsigned>(0, st.NumTruck());
      id_cliente = Random::Uniform<unsigned>(1, in.Customers() - 1);
    } while(st.GiaServito(id_cliente) || !(id_truck == st.NumTruck() || st.VediTruckResidualCapacity(id_truck) >= in.Demand(id_cliente))); //Negazione per non considerare la seconda parte dell'equazione logica
  
    st.Pushback(id_truck, id_cliente);
    if(id_truck != st.NumTruck())
      st.ChangeTruckResidualCapacity(id_truck, in.Demand(id_cliente));
    st.SetServito(id_cliente);  
  }

  //Aggiungo a tutti i truck il deposito finale
  for(unsigned i = 0; i < st.NumTruck() + 1; i++)
    st.Pushback(i, 0);

  //Trucco di gauss: inizializzazione vettore mosse
  st.TruccoGauss();
} 


void VRP_StateManager::GreedyState(VRP_State& st) 
{
  st.ResetState();

  int i_max, j_max;
  double guadagno_max, guadagno_temp = 0.0;
  bool esiste_cliente_valido = true;
  pair<int,int> first_reachable;
  bool res;

  //Imposto un pivot per ogni rotta (se non esiste un pivot CONVENIENTE da inserire non creo piu le rotte restanti)
  for(unsigned int i = 0; i < st.NumTruck(); i++)    //Escludo il truck con i clienti non serviti 
  {
    res = st.SetPivot(i);
    if(!res) 
      break;
  }

  //Fino a che trovo un cliente valido per un qualsiasi veicolo (cioè da un guadagno > 0, ha una domanda di carico accettabile dal veicolo, e !delivered)
  while(esiste_cliente_valido)
  {
    //cerco il primo veicolo e cliente conveniente servibile
    first_reachable = st.FirstNotDelivered();
    i_max = first_reachable.first;
    j_max = first_reachable.second;
    
    //se l' ho trovato
    if(first_reachable.first != -1)
    {
    //calcolo il guadagno (inteso come differenza tra il premio del cliente e la distanza tra la posizione corrente del veicolo e quella del cliente
    //il cliente trovato da anche un guadagno > 0 
      guadagno_max = st.CalculateProfit(i_max, j_max);
         
      for(unsigned int i = 0; i < st.NumTruck(); i++)
        for(unsigned int j = 1; j < in.Customers(); j++)
          //per ogni "coppia" veicolo-cliente se la capacita del veicolo è sufficente, se il cliente non è già stato servito, e se non stiamo considerando la stessa coppia veicolo-cliente calcolata all' inizio con la FirstNotDelivered()
          if((st.VediTruckResidualCapacity(i) - in.Demand(j) >= 0) && (static_cast<int>(i) != first_reachable.first || static_cast<int>(j) != first_reachable.second) && !st.GiaServito(j))
          {                 
          //calcolo il guadagno per servire il cliente j con la posizione corrente del veicolo i 
            guadagno_temp = st.CalculateProfit(i,j);  
           // (SOGLIA_Guadagno è una costante che vale 0)
            if(guadagno_temp > guadagno_max && guadagno_temp > in.GAIN_THRESHOLD)
            {
              guadagno_max = guadagno_temp;
              i_max = i;
              j_max = j;
            }
          }         

      st.Pushback(i_max, j_max);
      st.SetServito(j_max);
      st.ChangeTruckResidualCapacity(i_max, in.Demand(j_max));

      //sposto il veicolo i_max nella nuova posizione dove si trova il cliente che ho appena servito (j_max)
      st.MoveTruck(i_max, j_max);
    }
    else
    //se entro in questo else vuol dire che non ci sono piu cliente CONVENIENTI DA SERIVIRE 
      esiste_cliente_valido = false;
  }

  //inserisco i clienti non serviti nel truck finale 
  for(unsigned i = 1; i < st.NumClientiNonServiti(); i++)
    if(!st.GiaServito(i))
      st.Pushback(st.NumTruck(), i);

  //aggiungo il deposito finale 
  for(unsigned i = 0; i < st.NumTruck() + 1; i++)
    st.Pushback(i, 0);

  //Trucco di gauss: inizializzazione vettore mosse
  st.TruccoGauss();
}

bool VRP_StateManager::CheckConsistency(const VRP_State& st) const
{
  // Insert the code that checks if state in object st is consistent
  // (for debugging purposes)
  vector<bool> serviti(in.Customers(), false);

  for(unsigned i = 0; i < in.Trucks() + 1; i++)
  {
    // Per i truck diversi dal "truck" dei clienti non serviti la capacità deve essere rispettata e deve esserci il deposito (0) all'inizio e alla fine di ogni rotta
    if(i != in.Trucks() && (st.VediTruckResidualCapacity(i) < 0 || !(st.VediMatriceRoute(i, 0) == 0 || st.VediMatriceRoute(i, st.SizePath(i) - 1) == 0)))
      return false;
   
    for(unsigned j = 1; j < st.SizePath(i) - 1; j++)
    {
      // Per evitare clienti serviti più volte e con depot nel mezzo della rotta
      if(serviti[st.VediMatriceRoute(i, j)] || st.VediMatriceRoute(i, j) == 0)
        return false;

      serviti[st.VediMatriceRoute(i, j)] = true;
    }
  }
  // Deposito settato a "servito" perché presente ma non letto nel ciclo
  serviti[0] = true;

  // Tutti i clienti serviti
  for(unsigned k = 0; k < in.Customers(); k++)
    if(!serviti[k])
      return false;

  return true;
}

double VRP_DistanceCost::ComputeCost(const VRP_State& st) const
{
  double cost = 0.0;
  // Insert the code that computes the cost for component 1 of state st
  for(unsigned i = 0; i < st.NumTruck(); i++)
    for(unsigned j = 0; j < st.SizePath(i) - 1; j++)      //Messo -1 perché sono arrivato all'ultimo cliente della route (prima del deposito)
      cost += in.DistanzaCustomer(st.VediMatriceRoute(i, j), st.VediMatriceRoute(i, j + 1));
  return cost;
}


double VRP_Premi::ComputeCost(const VRP_State& st) const
{
  double reward = 0.0; 
  for(unsigned i = 0; i < st.NumTruck(); i++)
    for(unsigned j = 1; j < st.SizePath(i) - 1; j++)      //Messo -1 perché sono arrivato all'ultimo cliente della route (prima del deposito)
      reward -= in.Prize(st.VediMatriceRoute(i, j));
  return reward; 
}
          
void VRP_DistanceCost::PrintViolations(const VRP_State& st, ostream& os) const
{
  // Insert the code that prints the violations of component 1 
  // of state st	
  os << "Distance cost is " << ComputeCost(st) * in.FUEL_COST_LITER / in.AVG_TRUCK_CONS_LITER << endl; 
}

void VRP_Premi::PrintViolations(const VRP_State& st, ostream& os) const
{
  // Insert the code that prints the violations of component 1 
  // of state st	
  os << "Prize* is " << ComputeCost(st) << endl << "(* negative because opposite to the '+' sign of costs)" << endl;
}

/*****************************************************************************
 * Output Manager Methods
 *****************************************************************************/

void VRP_OutputManager::InputState(VRP_State& st, const VRP_Output& out) const 
{
  // Insert the code that "translates" an output object to a state object
  unsigned i, j;

  st.ResetState();
  for (i = 0; i < out.SelectedPaths() - 1; i++)
  {
    for (j = 1; j < out.CustomersPath(i) - 1; j++)
    {
      st.Pushback(i, out(i, j));
      st.ChangeTruckResidualCapacity(i, in.Demand(out(i, j)));
    }  
    st.Pushback(i, out(i, j));
  }

  for (j = 0; j < out.CustomersPath(i); j++)    
      st.Pushback(i, out(i, j));
  
  st.Pushback(i, 0);
}

  
void VRP_OutputManager::OutputState(const VRP_State& st, VRP_Output& out) const 
{
  // Insert the code that "translates" a state object to an output object
  unsigned i, j;

  for (i = 0; i < st.NumTruck(); i++)
  {
    out.ResetPath(i);
    for (j = 0; j < st.SizePath(i); j++)  
    {
      out.ChangeInPaths(i, st.VediMatriceRoute(i,j));  
    }
  }
  out.ResetPath(i);

  for(j = 1; j < st.SizePath(i) - 1; j++)
    out.ChangeInPaths(i, st.VediMatriceRoute(i, j));  
}

/*****************************************************************************
 * VRP_Move1 Neighborhood Explorer Methods
 *****************************************************************************/

void VRP_Move1NeighborhoodExplorer::RandomMove(const VRP_State& st, VRP_Move1& mv) const
{
  // insert the code that writes a random move on mv in state st
  //Fa riferimento al vettore mosse inizializzato in precedenza

  //Bilanciamento scelta del truck
  unsigned q = Random::Uniform<unsigned>(0, st.VediVettoreMosse(st.NumTruck() - 1) - 1); //q = numero mossa random tra tutte le mosse possibili
  unsigned i = 0; //inizializzo il truck della mossa che viene scelto in base all numero della mossa random appena definito
  unsigned temp;

  while(q >= st.VediVettoreMosse(i))  //cerco di capire in che truck sono (grazie al numero di mosse presenti nel vettore e grazie alla sua caratteristica incrementale)
    i++;

  mv.qualeroute = i;
	mv.cust1 = Random::Uniform<unsigned>(1, st.SizePath(i) - 2); // -2 per i depositi 
  mv.cust2 = Random::Uniform<unsigned>(1, st.SizePath(i) - 3); // -3 per i 2 depositi piu il cliente sopra

  //Per bilanciare il -3 sopra per evitare di escludere un caso a priori nell'assegnazione mv.cust2
  if(mv.cust2 >= mv.cust1)
    mv.cust2++;
  else
  {
    temp = mv.cust2;
    mv.cust2 = mv.cust1;
    mv.cust1 = temp;
  }
} 

void VRP_Move1NeighborhoodExplorer::MakeMove(VRP_State& st, const VRP_Move1& mv) const
{
  // Insert the code that modify the state st based on the application of move mv
  unsigned temp1, temp2;
  temp1 = st.VediMatriceRoute(mv.qualeroute, mv.cust1);
  temp2 = st.VediMatriceRoute(mv.qualeroute, mv.cust2);
  st.ChangeMatriceRoute(mv.qualeroute, mv.cust1, temp2);
  st.ChangeMatriceRoute(mv.qualeroute, mv.cust2, temp1);
}  

bool VRP_Move1NeighborhoodExplorer::FeasibleMove(const VRP_State& st, const VRP_Move1& mv) const
{
  // Insert the code that check is move mv is legal in state st 
  // (return true if legal, false otherwise)
  return !((mv.qualeroute > st.NumTruck()) || 
           (mv.cust1 >= st.SizePath(mv.qualeroute) - 1) ||
           (mv.cust1 == 0) ||
           (mv.cust2 >= st.SizePath(mv.qualeroute) - 1) ||
           (mv.cust2 == 0) ||
           (mv.cust1 == mv.cust2));
} 

void VRP_Move1NeighborhoodExplorer::FirstMove(const VRP_State& st, VRP_Move1& mv) const
{
  // Insert the code the generate the first move in the neighborhood and store it in mv
  mv.cust1 = 1;  
  mv.cust2 = 2;
  mv.qualeroute = 0; //Inizializzo al primo truck e poi lo setto al primo feasible

  while(st.SizePath(mv.qualeroute) < 4)  // 4 perchè non mi va bene avere solo 0 - cliente - 0 ma servono almeno 2 clienti 
    mv.qualeroute++;

}


bool VRP_Move1NeighborhoodExplorer::NextMove(const VRP_State& st, VRP_Move1& mv) const
{
  // Insert the code that generate the move that follows mv in the neighborhood, and writes
  // it back in mv. Return false if mv is already the last move. 
  if(mv.cust2 < st.SizePath(mv.qualeroute) - 2)
    mv.cust2++;
  else
  {
    mv.cust1++;
    mv.cust2 = mv.cust1 + 1;  // per evitare di avere lo swap tra cust1 e cust2 uguali
  }
  if(mv.cust1 > st.SizePath(mv.qualeroute) - 3) // 3 avere lo spazio anche per il cust2
  {
    mv.qualeroute++;
    while(st.SizePath(mv.qualeroute) < 4)  // messo 4 perchè non mi va bene avere solo 0 - cliente - 0 ma servono almeno 2 clienti 
      mv.qualeroute++;
    mv.cust1 = 1;
    mv.cust2 = 2;
  }
  
  if(mv.qualeroute >= st.NumTruck()) // ho finito le mosse (non ha senso swappare nel truck dei clienti non serviti)
    return false;

  return true;
}

double VRP_MoveDeltaDistanceCost::ComputeDeltaCost(const VRP_State& st, const VRP_Move1& mv) const
{
  // Insert the code that computes the delta cost of component 1 for move mv in state st
  double cost = 0.0;

  //VEDI ESEMPLIFICAZIONE - DISEGNO
  //Se cust1 e cust2 non sono "vicini" 
  if(mv.cust1 != mv.cust2 - 1)
  {
    cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust1), st.VediMatriceRoute(mv.qualeroute, mv.cust1 + 1));
    cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust2 - 1), st.VediMatriceRoute(mv.qualeroute, mv.cust2));
    cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust2), st.VediMatriceRoute(mv.qualeroute, mv.cust1 + 1));
    cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust2 - 1), st.VediMatriceRoute(mv.qualeroute, mv.cust1));
  }

  cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust1 - 1), st.VediMatriceRoute(mv.qualeroute, mv.cust1));
	cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust2), st.VediMatriceRoute(mv.qualeroute, mv.cust2 + 1));
  cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust1 - 1), st.VediMatriceRoute(mv.qualeroute, mv.cust2));
	cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.qualeroute, mv.cust1), st.VediMatriceRoute(mv.qualeroute, mv.cust2 + 1));

  return cost;
}

double VRP_MoveDeltaPremi::ComputeDeltaCost(const VRP_State& st, const VRP_Move1& mv) const
{
  return 0.0;
}

/*****************************************************************************
 * VRP_Move2 Neighborhood Explorer Methods
 *****************************************************************************/

void VRP_Move2NeighborhoodExplorer::RandomMove(const VRP_State& st, VRP_Move2& mv) const
{
  unsigned size = 0;

  for(unsigned i = 0; i < st.NumTruck(); i++)  //Guardo tutti i truck e restituisco quanti clienti serve il truck che ne serve di piu 
    if(st.SizePath(i) > size)                  
      size = st.SizePath(i);

  do 
  {
    mv.start_truck = Random::Uniform<unsigned>(0, st.NumTruck());         //truck di partenza (può essere incluso il truck dei clienti non serviti)
    mv.destination_truck = Random::Uniform<unsigned>(0, st.NumTruck());   //truck di arrivo (uguale)
    mv.cust = Random::Uniform<unsigned>(1, size - 2);                     //posizione del cliente nel truck di partenza: -2 perché considero al massimo fino al cliente prima del deposito
    mv.cust_dest_position = Random::Uniform<unsigned>(1, size - 1);       //posizione del cliente nel truck di arrivo: -1 perché posso spostare il cliente anche nella posizione del deposito in arrivo con questo che vienefatto scivolare in avanti

  } while(!FeasibleMove(st,mv));                                          //Tira a caso finché non è feasible

} 

void VRP_Move2NeighborhoodExplorer::MakeMove(VRP_State& st, const VRP_Move2& mv) const
{
  // Insert the code that modify the state st based on the application of move mv

  unsigned id_cliente_spostato;
  id_cliente_spostato = st.VediMatriceRoute(mv.start_truck, mv.cust);
  st.Erase(mv.start_truck, mv.cust);    
  st.Insert(mv.destination_truck, mv.cust_dest_position, id_cliente_spostato);
  
  if(mv.start_truck != st.NumTruck())
    st.ChangeTruckResidualCapacity(mv.start_truck, - in.Demand(id_cliente_spostato));
  
  if(mv.destination_truck != st.NumTruck())  
    st.ChangeTruckResidualCapacity(mv.destination_truck, in.Demand(id_cliente_spostato));  

  //Trucco di gauss
  st.TruccoGauss();
}  


bool VRP_Move2NeighborhoodExplorer::FeasibleMove(const VRP_State& st, const VRP_Move2& mv) const
{
  // Insert the code that check is move mv is legal in state st 
  // (return true if legal, false otherwise)

  return !((mv.cust_dest_position > st.SizePath(mv.destination_truck) - 1 ||                                             //Percontrollare la mossa di input dal menù di Easylocal
            (mv.start_truck == mv.destination_truck && mv.cust_dest_position > st.SizePath(mv.destination_truck) - 2) || //se stesso truck togliendo l'elemento scala tutto a sinistra quindi sempre per evitare di considerare il deposito
            mv.cust > st.SizePath(mv.destination_truck) - 1 ||
            mv.cust > st.SizePath(mv.start_truck) - 2 ||                                                                 //posizione -2 perché considero al massimo fino al cliente prima del deposito

            mv.start_truck > st.NumTruck() ||
            mv.destination_truck > st.NumTruck() ||
            mv.cust == 0 ||
            mv.cust_dest_position == 0) ||
           !(mv.destination_truck == st.NumTruck() || in.Demand(st.VediMatriceRoute(mv.start_truck,mv.cust)) <= st.VediTruckResidualCapacity(mv.destination_truck)) || 
            //Negazione per non considerare la seconda parte dell'equazione logica
            //Se non sono nel truck dei clienti non serviti allora la mossa è feasible solo se questa non erode più capacità di quella disponibile

           (mv.start_truck == mv.destination_truck && mv.cust >= mv.cust_dest_position) ||                               //Evito di considerare la stessa mossa due volte 
           (mv.start_truck == st.NumTruck() && st.SizePath(mv.start_truck) == 2));                                       //Per evitare di spostare il depot presente comunque nel truck dei clienti non serviti (quando questo ha dimensione 2)
} 

void VRP_Move2NeighborhoodExplorer::FirstMove(const VRP_State& st, VRP_Move2& mv) const
{
  // Insert the code the generate the first move in the neighborhood and store it in mv
  mv.cust = 1;  
  mv.start_truck = 0;
  mv.destination_truck = 0;
  mv.cust_dest_position = 1;

  while(st.SizePath(mv.start_truck) < 3)    //Per evitare di spostare i depositi 
    mv.start_truck++;
  
  while((st.SizePath(mv.start_truck) == 3 && mv.start_truck == mv.destination_truck) ||     //== 3 altrimenti faccio una mossa nulla (sposto un cliente nella sua stessa posizione iniziale)
        (st.VediTruckResidualCapacity(mv.destination_truck) < in.Demand(st.VediMatriceRoute(mv.destination_truck, mv.cust))))        
    mv.destination_truck++; 
    
  if(mv.start_truck == mv.destination_truck)
    mv.cust_dest_position = 2;
}

bool VRP_Move2NeighborhoodExplorer::NextMove(const VRP_State& st, VRP_Move2& mv) const
{
  do
    if (!AnyNextMove(st,mv))
      return false;
  while (!FeasibleMove(st,mv));
  return true;
}

bool VRP_Move2NeighborhoodExplorer::AnyNextMove(const VRP_State& st, VRP_Move2& mv) const
{
  // Insert the code that generate the move that follows mv in the neighborhood, and writes
  // it back in mv. Return false if mv is already the last move.  

  if((mv.cust_dest_position < st.SizePath(mv.destination_truck) - 2) ||                                          
     (mv.cust_dest_position == st.SizePath(mv.destination_truck) - 2 && mv.start_truck != mv.destination_truck))   //Se truck diversi basta il -1
    mv.cust_dest_position++;
  else
  {
    mv.cust_dest_position = 1; 
    if(mv.destination_truck < st.NumTruck())
      mv.destination_truck++;
    else
    {
      mv.destination_truck = 0;
      if (mv.cust < st.SizePath(mv.start_truck) - 2)
        mv.cust++;
      else
      {
        mv.cust = 1;
        if (mv.start_truck < st.NumTruck())
          do
          {
            mv.start_truck++;
          } while(st.SizePath(mv.start_truck) < 3 && mv.start_truck < st.NumTruck());   //while necessario per escludere il caso in cui muovo da un truck che ha solo 0 0 
        else
          return false; //ho finito le mosse
      }
    }
  }

  return true;
}

double VRP_MoveDeltaDistanceCost2::ComputeDeltaCost(const VRP_State& st, const VRP_Move2& mv) const
{
  // Insert the code that computes the delta cost of component 1 for move mv in state st
  double cost = 0.0;

  //VEDI ESEMPLIFICAZIONE - DISEGNO
  if(mv.start_truck != mv.destination_truck || mv.start_truck == st.NumTruck() || (mv.start_truck == mv.destination_truck && mv.cust_dest_position < mv.cust) )
  {
    if(mv.start_truck != st.NumTruck())
    {
      cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust - 1), st.VediMatriceRoute(mv.start_truck, mv.cust));
      cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust), st.VediMatriceRoute(mv.start_truck, mv.cust + 1));
      cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust - 1), st.VediMatriceRoute(mv.start_truck, mv.cust + 1));
    }
    if(mv.destination_truck != st.NumTruck())
    {
      cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position - 1), st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position));
      cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position - 1), st.VediMatriceRoute(mv.start_truck, mv.cust));
      cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust), st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position));
    }
  }
  else //if(mv.cust_dest_position > mv.cust)
  { 
    cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust - 1), st.VediMatriceRoute(mv.start_truck, mv.cust));
    cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust), st.VediMatriceRoute(mv.start_truck, mv.cust + 1));
    cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust - 1), st.VediMatriceRoute(mv.start_truck, mv.cust + 1));

    cost -= in.DistanzaCustomer(st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position), st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position + 1));
    cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position), st.VediMatriceRoute(mv.start_truck, mv.cust));
    cost += in.DistanzaCustomer(st.VediMatriceRoute(mv.start_truck, mv.cust), st.VediMatriceRoute(mv.destination_truck, mv.cust_dest_position + 1));
  }

  return cost;
}


double VRP_MoveDeltaPremi2::ComputeDeltaCost(const VRP_State& st, const VRP_Move2& mv) const
{
  double cost = 0.0; 

  if(mv.start_truck == st.NumTruck())
    cost -= in.Prize(st.VediMatriceRoute(mv.start_truck, mv.cust));  //metto -= perche voglio minimizzare

  if(mv.destination_truck == st.NumTruck())
    cost += in.Prize(st.VediMatriceRoute(mv.start_truck, mv.cust));

  return cost;
}


// ------------------------------------------------------------------------------------
