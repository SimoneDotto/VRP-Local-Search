// File VRP_Basics.hh
#ifndef VRP_BASICS_HH
#define VRP_BASICS_HH

#include "VRP_Data.hh"
#include <iomanip>

class VRP_State
{  
  friend ostream& operator<<(ostream& os, const VRP_State& st);
  friend bool operator==(const VRP_State& st1, const VRP_State& st2);

public:
  VRP_State(const VRP_Input &in);
  VRP_State& operator=(const VRP_State& s);
  void TruccoGauss();
  unsigned NumTruck() const { return in.Trucks(); }
  unsigned SizePath(unsigned i) const { return matrice_route[i].size(); }
  unsigned VediMatriceRoute(unsigned i, unsigned j) const { return matrice_route[i][j]; }
  void ChangeMatriceRoute(unsigned i, unsigned j, unsigned new_value) { matrice_route[i][j] = new_value; }
  unsigned VediVettoreMosse(unsigned i) const { return vettore_mosse[i]; }
  void SetVettoreMosse(unsigned i, unsigned v) { vettore_mosse[i] = v; }
  void Pushback(unsigned i, unsigned valore) { matrice_route[i].push_back(valore); }   //Nel costruttore mettiamo solo uno 0 e poi l altro lo aggiungiamo quando facciamo il primo stato 
  int VediTruckResidualCapacity(unsigned i) const { return truck_residual_capacity[i]; }
  void ChangeTruckResidualCapacity (unsigned i, unsigned v) { truck_residual_capacity[i] -= v; } 
  bool GiaServito(unsigned i) const { return gia_servito[i]; }
  void SetServito(unsigned i) { gia_servito[i] = true; }
  bool TuttiServiti();
  void ResetState();
  void Erase(unsigned i, unsigned p) { matrice_route[i].erase(matrice_route[i].begin() + p); }
  void Insert(unsigned i, unsigned p, unsigned id_cliente) { matrice_route[i].insert(matrice_route[i].begin() + p, id_cliente ); }

  //metodi per Greedy
  bool SetPivot (unsigned n_route);
  unsigned SearchIndexMinProfit(const vector<double> &guadagni);
  void MoveTruck (unsigned i_truck, unsigned id_cliente) { truck_position[i_truck] = id_cliente; }   //sposta il camion i-esimo nella posizione del cliente appena servito 
  unsigned GetPositionTruck(unsigned t) const { return truck_position[t]; }
  pair<int,int> FirstNotDelivered();
  double CalculateProfit(int truck_index, int customer_index) const;  //calcola guadagno
  unsigned NumClientiNonServiti() { return gia_servito.size(); }

protected:
  const VRP_Input & in;  
  vector<unsigned> vettore_mosse; 
  vector<vector<unsigned> > matrice_route; 
  vector<int> truck_residual_capacity;
  vector<bool> gia_servito;

  //Struttura per il Greedy: vettore in cui per ciascun camion indico la sua posizione attuale 
  vector<unsigned> truck_position;     
  
};

class VRP_Move1    //SWAP dentro un truck - SwapInTruck
{
  friend bool operator==(const VRP_Move1& m1, const VRP_Move1& m2);
  friend bool operator!=(const VRP_Move1& m1, const VRP_Move1& m2);
  friend bool operator<(const VRP_Move1& m1, const VRP_Move1& m2);
  friend ostream& operator<<(ostream& os, const VRP_Move1& c);
  friend istream& operator>>(istream& is, VRP_Move1& c);
 public:
  VRP_Move1();
  unsigned cust1, cust2; //Clienti che vengono scambiati
  unsigned qualeroute;   //Rappresenta il numero del truck nel quale sto facendo lo scambio
};

class VRP_Move2   //MOVE cliente da una posizione in un truck ad un (altro) truck in una (altra) posizione - ChangeInterTruck 
{
  friend bool operator==(const VRP_Move2& m1, const VRP_Move2& m2);
  friend bool operator!=(const VRP_Move2& m1, const VRP_Move2& m2);
  friend bool operator<(const VRP_Move2& m1, const VRP_Move2& m2);
  friend ostream& operator<<(ostream& os, const VRP_Move2& c);
  friend istream& operator>>(istream& is, VRP_Move2& c);
 public:
  VRP_Move2();
  unsigned cust;                //Posizione del cliente nel truck di "partenza"
  unsigned start_truck;         //Truck di partenza
  unsigned cust_dest_position;  //Posizione del cliente nel truck di "arrivo" 
  unsigned destination_truck;   //Truck di arrivo
};

#endif

