// File VRP_Data.cc
#include "VRP_Data.hh"
#include <fstream>


VRP_Input::VRP_Input(string file_name)
{
  // Insert the code that reads the input from the file and stored it into the data structures of the object 
  ifstream is(file_name);
  
  if(is.fail())
      throw runtime_error("Cannot open instance file");

  int x, y, customer_demand, time;
  double customer_prize;
  string bin;
   
  //ignore file intestation "Name chriX" and "MAXVEHICLES in the following line"
  is >> bin >> bin >> bin;

  //read the number of trucks available, ignore "MAXCAPACITY", read the max capacity of each vehicle and ignore the line about the time capacity (not used for this project)
  is >> trucks >> bin >> capacity >> bin >> tot_time;

  //ignore "DEPOT" and read the coordinates of the depot
  is >> bin >> x >> y;

  //ignore "CUSTOMERS" and read the number of customers
  is >> bin >> customers;     

  //add the depot to the different plants considered
  customers += 1;
  
  //ignore "CUSTOMERDATA"
  is >> bin;
  
  customer_map.resize(customers);
  demand.resize(customers);
  prize.resize(customers);
  customer_distances.resize(customers);
  service_times.resize(customers);
  for(unsigned i = 0; i < customers; i++)
    customer_distances[i].resize(customers);

  //set the data for the depot
  customer_map[0] = make_pair(x,y); 
  demand[0] = 0;
  prize[0] = 0;
  service_times[0] = 0;
  
  for(unsigned i = 1; i < customers; i++)
  {
    //read data for each customer and assign them to the different data structures
    is >> x >> y >> customer_demand >> time >> customer_prize;
    customer_map[i] = make_pair(x,y);
    demand[i] = customer_demand;
    prize[i] = customer_prize;
    service_times[i] = time;
  }

  for(unsigned i = 0; i < customers; i++)
    for(unsigned j = 0; j < customers; j++)
      customer_distances[i][j] = sqrt(pow(customer_map[i].first - customer_map[j].first, 2) + pow(customer_map[i].second - customer_map[j].second, 2));
}

ostream& operator<<(ostream& os, const VRP_Input& in)
{
  // Insert the code that write the input object (needed for debugging purposes)
  os << "NAME chri" << in.customers - 1 << endl << endl; 

  os << "MAXVEHICLES " << in.trucks << endl;
  os << "MAXCAPACITY " << in.capacity << endl;
  os << "MAXTIME " << in.tot_time << endl << endl;

  os << "DEPOT " << in.NodeX(0) << " " << in.NodeY(0) << endl;

  os << "CUSTOMERS " << in.customers - 1 << endl << endl; 

  os << "CUSTOMERSDATA" << endl;
  for(unsigned i = 1; i < in.customers; i++)
    os << setw(3) << in.NodeX(i) << setw(3) << in.NodeY(i) << setw(3) << in.Demand(i) << setw(3) << in.Time(i) << "    " << in.Prize(i) << endl;

  return os;
}


VRP_Output::VRP_Output(const VRP_Input& my_in)
  : in(my_in), paths(my_in.Trucks() + 1)
{}


VRP_Output& VRP_Output::operator=(const VRP_Output& out)	
{
  // Insert the code that copies all data structures of the
  // output object from the ones of the parameter out
  // (excluding the reference to the input object, that is constant)
  paths = out.paths;
  return *this;
}

ostream& operator<<(ostream& os, const VRP_Output& out)
{ 
 // Insert the code that writes the output object
  unsigned i, j;

  for (i = 0; i < out.paths.size() - 1; i++)
  {
    os << "Truck " << i << ": ";
    for (j = 0; j < out.paths[i].size(); j++)
      os << " " << out.paths[i][j]; 
    os << endl;
  }

  os << "ID dei clienti non serviti: ";
  for(j = 0; j < out.paths[i].size(); j++)
    os << " " << out.paths[i][j]; 
  
  os << endl;
  
  return os;
}

istream& operator>>(istream& is, VRP_Output& out)
{
  // Insert the code that reads the output object

  unsigned i, j, num;
  unsigned cnt = 0;    //contatore per sapere quanti sono i clienti serviti e di conseguenza dedurre quanti sono i non serviti 
  string bin_truck;
  char ch;

  out.paths.clear();  
  out.paths.resize(out.in.Trucks() + 1);
  
  for(i = 0; i < out.paths.size() - 1; i++)
  {
    is >> bin_truck >> num >> ch >> num; 
    out.paths[i].push_back(num);  

    do
    {
      is >> num;
      out.paths[i].push_back(num); 
      cnt++; 
    } while(num != 0);

    cnt--; //per non incrementare una volta raggiunto lo zero finale
  }
  
  is >> bin_truck >> bin_truck >> bin_truck >> bin_truck>> bin_truck;
  
  for(j = 0; j < out.in.Customers() - cnt - 1; j++)
  {
    is >> num;
    out.paths[i].push_back(num);
  }

  return is; 
}

  