#! /usr/bin/env python
import os
from datetime import date
import random
import math



# list of names of instances
instances = ["p03", "p06", "p07", "p08", "p09", "p10", "p13", "p14", "p16"]
# number of repetitions
repetitions = 1

# directory of the instances
instance_directory = "Instances"

# directory of the solutions
solution_directory = "Solutions"
solution_extension = "sol"

# ALGORITHM PARAMETERS
# Hill Climbing has 1 parameter: max_idle_iterations
max_idle_iteration_array = [100, 1000, 1000000]

today = date.today()
# The log file stores a summary of the results
log_file = "Log_HC" + today.isoformat() + ".log"
f = open(log_file,"w")
f.write("Trial Instance Move Cost Seed Time Max_idle_Iterations\n")
f.close()

for i in range(repetitions):
  for instance in instances:
    in_file_path = instance_directory + "/" + instance + ".cri"
    if os.path.isfile(in_file_path): # Check if the file exists 
      for move in range(1,3):
        for mi in range(len(max_idle_iteration_array)):
            seed = random.randint(0,100000000)
            max_idle_iteration = max_idle_iteration_array[mi]
            if not os.path.isdir(solution_directory):
                os.mkdir(solution_directory) 
            
            # the solution file
            sol_file = solution_directory + "/sol-HC"+ str(move) +"-" + instance + \
                "-" + str(mi) + "_" + \
                str(i) + "." + solution_extension
            
            # command line
            command = "./vrp" + \
            " --main::instance " +  in_file_path  + \
            " --main::method HC" + str(move) + \
            " --VRP_MoveHillClimbing" + str(move) + "::max_idle_iterations " + str(max_idle_iteration) + \
            " --main::output_file " + sol_file
                
            print(command + "\n")
            os.system(command)

            f = open(sol_file,'r')
            cost_string = f.readline()
            time_string = f.readline()

            cost = str.rstrip(cost_string.split(" ")[1],'\n') # take only the second part (index 1)
            time = str.rstrip(time_string.split(" ")[1],'\n')
            f = open(log_file,"a")
            f.write(str(i) + " " + instance +" HC" + str(move) + " " + \
                str(cost) + " " + str(seed) + " " + str(time) + " " + \
                str(max_idle_iteration) + "\n")
            f.close()

