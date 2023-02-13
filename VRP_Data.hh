// File VRP_Data.hh
#ifndef VRP_DATA_HH
#define VRP_DATA_HH

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <iomanip>

using namespace std;

class VRP_Input 
{
    friend ostream& operator<<(ostream& os, const VRP_Input& in);

  public:
    VRP_Input(string file_name);
    // Insert your getters
    //eventually time capacity and individual service time
    
    unsigned Customers() const{return customers; }
    unsigned Trucks() const {return trucks; }
    unsigned Capacity() const {return capacity; }
    unsigned TotTime() const {return tot_time; }

    
    pair<int,int> GetCustomerPosition (unsigned i) const {return customer_map[i];}
    int NodeX (unsigned i) const {return customer_map[i].first; }
    int NodeY (unsigned i) const {return customer_map[i].second; }
    double DistanzaCustomer(unsigned i, unsigned j) const {return customer_distances[i][j]; } 
    int Demand (unsigned i) const {return demand[i]; }
    double Prize (unsigned i) const {return prize[i]; }
    unsigned Time (unsigned i) const {return service_times[i]; }

    //COSTANTI IPOTIZZATE
    const double FUEL_COST_LITER = 1.39;         //euro
    const double AVG_TRUCK_CONS_LITER = 13;      //km con un litro
    const double GAIN_THRESHOLD = 0.0;           

  protected:
    // Insert your data members
    unsigned customers, trucks, capacity, tot_time;    //customers, trucks number, vehicle capacity and service time (not used)
    vector <pair<int,int> > customer_map;              //x and y coordinates of each customer
    vector <int> demand;                               //demand from each customer 
    vector <double> prize;                             //profits from each customer
    vector<vector<double> > customer_distances;        //customer distances matrix
    vector <unsigned> service_times;                   //(not used)
};


class VRP_Output 
{
  friend ostream& operator<<(ostream& os, const VRP_Output& out);
  friend istream& operator>>(istream& is, VRP_Output& out);
public:
  VRP_Output(const VRP_Input& in);
  VRP_Output& operator=(const VRP_Output& out);

  // Insert your getters
  unsigned operator()(unsigned i, unsigned j) const { return paths[i][j]; }
  unsigned& operator()(unsigned i, unsigned j) { return paths[i][j]; }

  unsigned CustomersPath(unsigned i) const { return paths[i].size(); }
  unsigned SelectedPaths() const { return paths.size(); }
  void ResetPath(unsigned i) { paths[i].clear(); } 
  void ChangeInPaths(unsigned i, unsigned new_value) { paths[i].push_back(new_value); }

protected:
  const VRP_Input& in;
  // Insert your data members
  vector<vector<unsigned> > paths;     //(truck x customer) for each truck memorize the index of the customer for the correspondent order it is served by the truck, starting and ending with 0 												
};




#endif
