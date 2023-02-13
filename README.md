# VRP-Local-Search

The Vehicle Routing Problem with Capacitance (VRPC) is a variant of the classic Vehicle Routing Problem (VRP), which seeks to find the optimal set of routes for a fleet of vehicles to visit a set of customers while minimizing the total travel distance and ensuring that the capacity constraints of each vehicle are not exceeded.
In VRPC with prizes, in addition to the capacity constraints, there is a prize associated with each customer, and the objective is to maximize the total prize collected by the vehicles while minimizing the travel cost. Customers may also not be served if the profit (understood as the difference between the cost of transport from the current location of the truck to that of the customer minus the price the customer pays) doesn't apport a positive earn. 

# Local Search
Local Search is a metaheuristic optimization technique that iteratively improves a candidate solution by making small changes to the solution and accepting the changes if they result in an improvement. In the context of VRPC, a local search algorithm starts with an initial solution (that could be Greedy or Random) and repeatedly makes small changes to the routes, such as swapping customers or moving customers from one route to another, until a satisfactory solution is found. 

# VRP_Data files
* The VRP_Input class contains information about the input data for a Vehicle Routing Problem (VRP), including the number of customers and trucks, the capacity of each truck, the coordinates and demand of each customer, and the prize for serving each customer. It also contains a matrix of customer distances and an array of service times.
* The VRP_Output class stores the solution of the VRP, which is represented as a matrix of paths where each row represents a truck and the indices of customers that truck serves. The class also contains functions for accessing and modifying the data stored in the matrix. The VRP_Output class takes a VRP_Input object as a constructor parameter to ensure that the solution is consistent with the input data.

## VRP_Basic files 
The VRP_State class represents the state of a solution, including data structures to keep track of routes, truck positions, and the delivery status of clients. It also has methods to manipulate this state, such as resetting it, changing the delivery status of a client, and erasing or inserting clients in a route. The VRP_State class also has methods for the greedy algorithm, such as calculating the profit of a move, searching for the minimum profit, and moving a truck.
* The **VRP_Move1** class represents a swap move between two clients within a truck.
* The **VRP_Move2** class represents a move between two clients, one from a position in a truck to another truck in another position.
Both VRP_Move1 and VRP_Move2 classes have methods to compare, output, and input their objects.

## VRP_Helpers files
* VRP_StateManager: A state manager class that initializes a VRP state, checks its consistency and performs a greedy state construction.
* VRP_DistanceCost and VRP_Premi: Two cost components that calculate the cost of a state based on the distance and premium cost, respectively.
* VRP_MoveDeltaDistanceCost and VRP_MoveDeltaPremi: Two classes that calculate the change in cost when a move is made.
* VRP_Move1NeighborhoodExplorer and VRP_Move2NeighborhoodExplorer: Two neighborhood explorer classes that implement a set of moves for exploring the neighborhood of a state and finding a better solution.



# VRP Input File

NAME chri100

MAXVEHICLES 15
MAXCAPACITY 200
MAXTIME 200

DEPOT 35 35

CUSTOMERS 100

CUSTOMERDATA
41 49 10 10 7.00
35 17 7 10 5.00
55 45 13 10 10.00
55 20 19 10 13.00
15 30 26 10 31.00
25 30 3 10 3.00
20 50 5 10 6.00
10 43 9 10 11.00
55 60 16 10 13.00
30 60 16 10 10.00
20 65 12 10 16.00
50 35 19 10 10.00
30 25 23 10 23.00
15 10 20 10 23.00
