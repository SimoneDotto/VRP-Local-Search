#include "VRP_Helpers.hh"

using namespace EasyLocal::Debug;

int main(int argc, const char* argv[])
{
  ParameterBox main_parameters("main", "Main Program options");

  // The set of arguments added are the following:
  Parameter<string> instance("instance", "Input instance", main_parameters); 
  Parameter<int> seed("seed", "Random seed", main_parameters);
  Parameter<string> method("method", "Solution method (empty for tester)", main_parameters);   
  Parameter<string> init_state("init_state", "Initial state (to be read from file)", main_parameters);
  Parameter<string> output_file("output_file", "Write the output to a file (filename required)", main_parameters);
 
  CommandLineParameters::Parse(argc, argv, false, true);  

  if (!instance.IsSet())
  {
    cout << "Error: --main::instance filename option must always be set" << endl;
    return 1;
  }
  VRP_Input in(instance);

  if (seed.IsSet())
    Random::SetSeed(seed);
  
  // Cost components: second parameter is the cost, third is the type (true -> hard, false -> soft)
  VRP_DistanceCost cc1(in, (in.FUEL_COST_LITER/in.AVG_TRUCK_CONS_LITER), false);
  VRP_Premi cc2(in, 1, false);     
 
  // Delta cost Move 1
  VRP_MoveDeltaDistanceCost dcc1_1(in, cc1);
  VRP_MoveDeltaPremi dcc2_1(in, cc2);

  // Delta cost Move 2
  VRP_MoveDeltaDistanceCost2 dcc1_2(in, cc1);
  VRP_MoveDeltaPremi2 dcc2_2(in, cc2);

  // Helpers
  VRP_StateManager VRP_sm(in);
  VRP_Move1NeighborhoodExplorer VRP_nhe(in, VRP_sm);    // Move 1
  VRP_Move2NeighborhoodExplorer VRP_nhe2(in, VRP_sm);   // Move 2

  VRP_OutputManager VRP_om(in);
  
  // All cost components must be added to the state manager
  VRP_sm.AddCostComponent(cc1);
  VRP_sm.AddCostComponent(cc2);
  
  // All delta cost components must be added to the neighborhood explorer
  // Move 1
  VRP_nhe.AddDeltaCostComponent(dcc1_1);
  VRP_nhe.AddDeltaCostComponent(dcc2_1);
  
  // Move 2
  VRP_nhe2.AddDeltaCostComponent(dcc1_2);
  VRP_nhe2.AddDeltaCostComponent(dcc2_2);

  // Runners Move 1
  HillClimbing<VRP_Input, VRP_State, VRP_Move1> VRP_hc_1(in, VRP_sm, VRP_nhe, "VRP_MoveHillClimbing1");
  SteepestDescent<VRP_Input, VRP_State, VRP_Move1> VRP_sd_1(in, VRP_sm, VRP_nhe, "VRP_MoveSteepestDescent1");
  SimulatedAnnealing<VRP_Input, VRP_State, VRP_Move1> VRP_sa_1(in, VRP_sm, VRP_nhe, "VRP_MoveSimulatedAnnealing1");

  // Runners Move 2
  HillClimbing<VRP_Input, VRP_State, VRP_Move2> VRP_hc_2(in, VRP_sm, VRP_nhe2, "VRP_MoveHillClimbing2");
  SteepestDescent<VRP_Input, VRP_State, VRP_Move2> VRP_sd_2(in, VRP_sm, VRP_nhe2, "VRP_MoveSteepestDescent2");
  SimulatedAnnealing<VRP_Input, VRP_State, VRP_Move2> VRP_sa_2(in, VRP_sm, VRP_nhe2, "VRP_MoveSimulatedAnnealing2");

  // Tester Move 1
  Tester<VRP_Input, VRP_Output, VRP_State> tester(in,VRP_sm,VRP_om);
  MoveTester<VRP_Input, VRP_Output, VRP_State, VRP_Move1> swap_move_test(in,VRP_sm,VRP_om,VRP_nhe, "SwapInTruck", tester); 

  // Tester Move 2 
  MoveTester<VRP_Input, VRP_Output, VRP_State, VRP_Move2> mossa_finale_test(in,VRP_sm,VRP_om,VRP_nhe2, "ChangeInterTruck", tester);

  SimpleLocalSearch<VRP_Input, VRP_Output, VRP_State> VRP_solver(in, VRP_sm, VRP_om, "VRP_solver");
  if (!CommandLineParameters::Parse(argc, argv, true, false))
    return 1;


  if (!method.IsSet())
  { // If no search method is set -> enter in the tester
    if (init_state.IsSet())
      tester.RunMainMenu(init_state);
    else
      tester.RunMainMenu();
  }
  else
  {
    if (method == string("SA1"))
    {
      VRP_solver.SetRunner(VRP_sa_1);
    }
    else if (method == string("SA2"))
    {
      VRP_solver.SetRunner(VRP_sa_2);
    }
    else if (method == string("HC1"))
    {
      VRP_solver.SetRunner(VRP_hc_1);
    }
    else if (method == string("HC2"))
    {
      VRP_solver.SetRunner(VRP_hc_2);
    }
    else if (method == string("SD1"))
    {
      VRP_solver.SetRunner(VRP_sd_1);
    }
    else if (method == string("SD2"))
    {
      VRP_solver.SetRunner(VRP_sd_2);
    }
    else
    {
      cerr << "unrecognized method " << string(method) << endl;
      exit(1);
    }

    auto result = VRP_solver.Solve();
    // result is a tuple: 0: solutio, 1: number of violations, 2: total cost, 3: computing time
    VRP_Output out = result.output;
    if (output_file.IsSet())
    { // write the output on the file passed in the command line
      ofstream os(static_cast<string>(output_file).c_str());
      os << "Cost: " << result.cost.total  << endl;
      os << "Time: " << result.running_time << "s " << endl;
      os << out << endl;

      os.close();
    }
    else
    { // write the solution in the standard output    
      cout << "Cost: " << result.cost.total << endl;
      cout << "Time: " << result.running_time << "s " << endl;
      cout << out << endl;
					
    }
  }
  return 0;
}
