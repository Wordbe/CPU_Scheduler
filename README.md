# CPU_Scheduler
CPU Scheduling Simulation, and Evaluation

12 kinds of CPU schedulings are included.

   usage:

    'simulate' function (n_process, scheduling_type, time_quantum)
    Scheduling Type:
    1 : FCFS
    2 : NonPreemptive SJF
    3 : Preempiive SJF
    4 : Nonpreemptive Priority
    5 : Preemptive Priority
    6 : RR (Round Robin)
    7 : Lottery
    8 : HRN
    9 : Multilevel Queue
    10 : Multilevel Feedback Queue
    11 : NonPreemptive LIF
    12 : Preemptive LIF

# pre-requisite
gcc

# getting started
gcc main.c simulate.c create_test_processes.c -o main.out

./main.out

