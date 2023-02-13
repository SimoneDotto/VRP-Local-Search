#! /usr/bin/env python
import os
from datetime import date
import random
import math



# list of names of instances
instances = ["p03", "p06", "p07", "p08", "p09", "p10", "p13", "p14", "p16" ] 
# number of repetitions
repetitions = 1

# directory of the instances
instance_directory = "Instances"

# directory of the solutions
solution_directory = "Solutions"
solution_extension = "sol"

# total number of iterations
total_iterations = 1.0E07

# ALGORITHM PARAMETERS
# Simulated Annealing has 3 parameters: start temperature, temperature range and cooling rate
start_temp_array = [5.0, 10.0, 20.0]  
min_temp_array = [0.05, 0.1, 0.2]
cooling_rate_array = [0.95, 0.99, 0.995] 

today = date.today()
# The log file stores a summary of the results
log_file = "Log_SA" + today.isoformat() + ".log"
f = open(log_file,"w")
f.write("Trial Instance Move Cost Seed Time Start_Temp Min_Temp Cooling_Rate\n")
f.close()

for i in range(repetitions):
  for instance in instances:
    in_file_path = instance_directory + "/" + instance + ".cri"
    if os.path.isfile(in_file_path): # Check if the file exists 
      for move in range(1,3):
        for st in range(len(start_temp_array)):
          for mt in range(len(min_temp_array)):
            for cr in range(len(cooling_rate_array)):
                seed = random.randint(0,100000000)
                start_temp = start_temp_array[st]
                min_temp = min_temp_array[mt]
                cooling_rate = cooling_rate_array[cr]
                number_of_temperatures = math.log(min_temp/start_temp)/math.log(cooling_rate)              
                neighbors_sampled = int(round(total_iterations/number_of_temperatures))
                

                if not os.path.isdir(solution_directory):
                    os.mkdir(solution_directory) 
                
                # the solution file
                sol_file = solution_directory + "/sol-SA"+ str(move) +"-" + instance + \
                    "-" + str(st) + "-" + str(mt) + "-" + str(cr)  + "_" + \
                    str(i) + "." + solution_extension
                
                # command line
                command = "./vrp" + \
                " --main::instance " +  in_file_path  + \
                " --main::method SA" + str(move) + \
                " --VRP_MoveSimulatedAnnealing" + str(move) + "::start_temperature " + str(start_temp) + \
                " --VRP_MoveSimulatedAnnealing" + str(move) + "::min_temperature " + str(min_temp) + \
                " --VRP_MoveSimulatedAnnealing" + str(move) + "::cooling_rate " + str(cooling_rate) + \
                " --VRP_MoveSimulatedAnnealing" + str(move) + "::neighbors_sampled " + str(neighbors_sampled) + \
                " --main::output_file " + sol_file
                
                print(command + "\n")
                os.system(command)

                f = open(sol_file,'r')
                cost_string = f.readline()
                time_string = f.readline()

                cost = str.rstrip(cost_string.split(" ")[1],'\n') # take only the second part (index 1)

                time = str.rstrip(time_string.split(" ")[1],'\n')
                f = open(log_file,"a")
                f.write(str(i) + " " + instance +" SA" + str(move) + " " + \
                    str(cost) + " " + str(seed) + " " + str(time) + " " + \
                    str(start_temp) + " " + str(min_temp) + " " + str(cooling_rate) + "\n")
                f.close()

