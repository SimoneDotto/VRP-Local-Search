# VRP-Local-Search

The Vehicle Routing Problem with Capacitance (VRPC) is a variant of the classic Vehicle Routing Problem (VRP), which seeks to find the optimal set of routes for a fleet of vehicles to visit a set of customers while minimizing the total travel distance and ensuring that the capacity constraints of each vehicle are not exceeded.
In VRPC with prizes, in addition to the capacity constraints, there is a prize associated with each customer, and the objective is to maximize the total prize collected by the vehicles while minimizing the travel cost. Customers may also not be served if the profit (understood as the difference between the cost of transport from the current location of the truck to that of the customer minus the price the customer pays) doesn't apport a positive earn. 

# Local Search
Local Search is a metaheuristic optimization technique that iteratively improves a candidate solution by making small changes to the solution and accepting the changes if they result in an improvement. In the context of VRPC, a local search algorithm starts with an initial solution (that could be Greedy or Random) and repeatedly makes small changes to the routes, such as swapping customers or moving customers from one route to another, until a satisfactory solution is found. 

## VRP_basic files 
The VRP_State class represents the state of a solution, including data structures to keep track of routes, truck positions, and the delivery status of clients. It also has methods to manipulate this state, such as resetting it, changing the delivery status of a client, and erasing or inserting clients in a route. The VRP_State class also has methods for the greedy algorithm, such as calculating the profit of a move, searching for the minimum profit, and moving a truck.
* The **VRP_Move1** class represents a swap move between two clients within a truck.
* The **VRP_Move2** class represents a move between two clients, one from a position in a truck to another truck in another position.
Both VRP_Move1 and VRP_Move2 classes have methods to compare, output, and input their objects.
