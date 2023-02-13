// File VRP_Basics.cc
#include "VRP_Basics.hh"

VRP_State::VRP_State(const VRP_Input &my_in) 
  : in(my_in),  
    vettore_mosse(my_in.Trucks()),
    matrice_route(my_in.Trucks() + 1),      //Dim +1 per il truck con i clienti non serviti 
    truck_residual_capacity(my_in.Trucks()), 
    gia_servito(my_in.Customers()),
    truck_position(my_in.Trucks())
{} 


VRP_State& VRP_State::operator=(const VRP_State& st)
{
  // Insert the code that copies all data structures of the
  // state object from the ones of the parameter out
  // (excluding the reference to the input object, that is constant)
  vettore_mosse = st.vettore_mosse;
  matrice_route = st.matrice_route;
  truck_residual_capacity = st.truck_residual_capacity;
  gia_servito = st.gia_servito;
  truck_position = st.truck_position;
  return *this;
}

bool operator==(const VRP_State& st1, const VRP_State& st2)
{
  // Insert the code that compares two states (this is used by the tester)
  return st1.vettore_mosse == st2.vettore_mosse
      && st1.matrice_route == st2.matrice_route
      && st1.truck_residual_capacity == st2.truck_residual_capacity
      && st1.gia_servito == st2.gia_servito
      && st1.truck_position == st2.truck_position; 
}


ostream& operator<<(ostream& os, const VRP_State& st)
{
  // Insert the code that writes the state object (for debugging)
  //ricorda di allineare la stampa intestazione con valori stato
  os << "VRP_State: " << endl <<endl;
  os << "Matrice route : " << endl;
  for(unsigned i = 0; i < st.NumTruck(); i++)
  {
    for(unsigned j = 0; j < st.SizePath(i); j++)
      os << st.VediMatriceRoute(i, j) << " ";
    os << " capacita residua : "<<st.VediTruckResidualCapacity(i);
    os << endl;
  }

  for(unsigned j = 0; j < st.SizePath(st.NumTruck()); j++)
      os << st.VediMatriceRoute(st.NumTruck(), j) << " ";

  return os;
}


bool VRP_State::TuttiServiti()
{
  for(unsigned i = 1; i < gia_servito.size(); i++)    //Parto da 1 altrimenti mi conta il deposito che sarà sempre false
    if(!gia_servito[i])
      return false;
  return true;
}


void VRP_State::ResetState()
{
  unsigned i;
  for(i = 0; i < in.Trucks(); i++)
  {
    truck_residual_capacity[i] = in.Capacity();
    matrice_route[i].clear();
    matrice_route[i].push_back(0);
    vettore_mosse[i] = 0;
  }

  matrice_route[i].clear();
  matrice_route[i].push_back(0); //Truck clienti non serviti

  for(i = 0; i < in.Customers(); i++)
    gia_servito[i] = false; 
}


//##########################################################################################################################################
//##########################################################################################################################################

VRP_Move1::VRP_Move1() //SWAP
{
  // Insert the code that initialize the move
  cust1 = 0;
  cust2 = 0;
  qualeroute = 0;
}

bool operator==(const VRP_Move1& mv1, const VRP_Move1& mv2)
{
  // Insert the code that checks if two moves are identical
  return mv1.cust1 == mv2.cust1
    && mv1.cust2 == mv2.cust2
    && mv1.qualeroute == mv2.qualeroute;
}

bool operator!=(const VRP_Move1& mv1, const VRP_Move1& mv2)
{
  // Insert the code that checks if two moves are different
  return mv1.cust1 != mv2.cust1
    || mv1.cust2 != mv2.cust2
    || mv1.qualeroute != mv2.qualeroute;
}

bool operator<(const VRP_Move1& mv1, const VRP_Move1& mv2)
{
  // Insert the code that checks if one move precedes another one
  // (in any selected order)
  return (mv1.cust1 < mv2.cust1)
    || (mv1.cust1 == mv2.cust1 && mv1.cust2 < mv2.cust2)
    || (mv1.cust1 == mv2.cust1 && mv1.cust2 == mv2.cust2 && mv1.qualeroute < mv2.qualeroute);
}

istream& operator>>(istream& is, VRP_Move1& mv)
{
  // Insert the code that read a move
  char ch;
  return is >> mv.qualeroute >> ch >> mv.cust1 >> ch >> ch >> ch >> mv.cust2;
}

ostream& operator<<(ostream& os, const VRP_Move1& mv)
{
  // Insert the code that writes a move
  return os << mv.qualeroute << ") " << mv.cust1 << " <-> " << mv.cust2;  //Format example: 3) 1 <-> 2
}


//##########################################################################################################################################
//##########################################################################################################################################


VRP_Move2::VRP_Move2() //MOVE
{
  // Insert the code that initialize the move
  cust = 0;
  start_truck = 0;
  destination_truck = 0;
  cust_dest_position = 0;
}

bool operator==(const VRP_Move2& mv1, const VRP_Move2& mv2)
{
  // Insert the code that checks if two moves are identical
  return mv1.cust == mv2.cust
    && mv1.start_truck == mv2.start_truck
    && mv1.destination_truck == mv2.destination_truck
    && mv1.cust_dest_position == mv2.cust_dest_position;
}

bool operator!=(const VRP_Move2& mv1, const VRP_Move2& mv2)
{
  // Insert the code that checks if two moves are different
  return mv1.cust != mv2.cust
    || mv1.start_truck != mv2.start_truck
    || mv1.destination_truck != mv2.destination_truck
    || mv1.cust_dest_position != mv2.cust_dest_position;
}

bool operator<(const VRP_Move2& mv1, const VRP_Move2& mv2)
{
  // Insert the code that checks if one move precedes another one
  // (in any selected order)
  return (mv1.cust_dest_position < mv2.cust_dest_position)
    || (mv1.cust_dest_position == mv2.cust_dest_position && mv1.destination_truck < mv2.destination_truck)
    || (mv1.cust_dest_position == mv2.cust_dest_position && mv1.destination_truck == mv2.destination_truck && mv1.cust < mv2.cust)
    || (mv1.cust_dest_position == mv2.cust_dest_position && mv1.destination_truck == mv2.destination_truck && mv1.cust == mv2.cust && mv1.start_truck < mv2.start_truck);
}

istream& operator>>(istream& is, VRP_Move2& mv)
{
  // Insert the code that read a move
  char ch;
  string bin;
  return is >> ch >> mv.start_truck >> ch >> mv.cust >> ch >> bin >> ch >> mv.destination_truck >> ch >> mv.cust_dest_position >>ch;
}

ostream& operator<<(ostream& os, const VRP_Move2& mv)
{
  // Insert the code that writes a move
  //Format example: (1, 4) --> (3, 5): custumer nel truck 1 in posizione 4 viene spostato nel truck di destinazione 3 nella posizione 5
  return os << "( " <<mv.start_truck << " , " << mv.cust << " ) --> ( "<< mv.destination_truck << " , " << mv.cust_dest_position << " )"; 
}

//Metodi per Greedy
bool VRP_State::SetPivot (unsigned n_route) 
{
  vector<double> guadagni;
  guadagni.resize(in.Customers());
  unsigned i_min;

  for(unsigned i = 1; i < guadagni.size(); i++)
    guadagni[i] = in.Prize(i) - in.DistanzaCustomer(0, i) * in.FUEL_COST_LITER / in.AVG_TRUCK_CONS_LITER; 

  i_min = SearchIndexMinProfit(guadagni);
  if(i_min != 0)
  {
    SetServito(i_min);
    MoveTruck(n_route, i_min);  
    truck_residual_capacity[n_route] -= in.Demand(i_min);
    Pushback(n_route,i_min);   //AddMove
    return true;
  }
  
  return false;
}


unsigned VRP_State::SearchIndexMinProfit(const vector<double> &guadagni)
{
  unsigned i_min = 0;
  for(unsigned i = 1; i < guadagni.size() && i_min == 0; i++)
    if(guadagni[i] > in.GAIN_THRESHOLD && !GiaServito(i))
      i_min = i;    
  
  for(unsigned j = 1; j < guadagni.size() && i_min != 0 ; j++)
    if(guadagni[j] < guadagni[i_min] && guadagni[j] > in.GAIN_THRESHOLD && !GiaServito(j))
      i_min = j;

  return i_min;
}
   

pair<int,int> VRP_State::FirstNotDelivered()
{
  //restituisce la prima coppia veicolo cliente conveniente da servire (cioè tale cliente rispetto alla posizone attuale del camion è conveniente da servire)
  double d, g;
  for(unsigned int i = 0; i < in.Trucks(); i++)
    for(unsigned int j = 1; j < gia_servito.size(); j++)
    {
      d = in.DistanzaCustomer(GetPositionTruck(i), j);
      g = in.Prize(j) - (d * in.FUEL_COST_LITER) / in.AVG_TRUCK_CONS_LITER;
      if(!gia_servito[j] && g > in.GAIN_THRESHOLD && (truck_residual_capacity[i] - in.Demand(j) >= 0))  
        return make_pair(i, j);
    }
  
  return make_pair(-1, -1);
}


double VRP_State::CalculateProfit(int truck_index, int customer_index) const
{
  //calcola il guadagno inteso come differenza tra il premio che il cliente fornisce e il costo di carburante per arrivare alla posizione del cliente dalla posizione attuale del camion
  double dist, premio;
  dist = in.DistanzaCustomer(GetPositionTruck(truck_index), customer_index);
  premio = in.Prize(customer_index);
  return premio - (dist * in.FUEL_COST_LITER) / in.AVG_TRUCK_CONS_LITER;
}

//Trucco di gauss: inizializzazione vettore mosse
  //Vettore mosse --> struttura usata per il random move della prima mossa (SWAP) che grazie al trucco di gauss viene inizializzata al numero totale di mosse possibili per ogni truck
void VRP_State::TruccoGauss()
{
  int c, v = 0; 
  for(unsigned i = 0; i < NumTruck(); i++) //esclude truck clienti non serviti (per cui per la prima mossa non ha sensofare lo swap)
  {
    c = SizePath(i) - 3;           
    v += (1 + c) / 2.0 * c;                
    SetVettoreMosse(i, v);   //Vettore mosse "incrementale": il truck i+1 esimo presenta le mosse proprie e del truck i esimo
  }
}