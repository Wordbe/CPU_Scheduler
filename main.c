#include "simulation.h"

/* 
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
*/

int main(int argc, char *argv[]) {
    // int n_process = atoi(argv[1]);
    // int n_io = atoi(argv[2]);
    int n_process = 9;
    int n_io = 2;

    // create_processes(n_process, n_io);
    create_test_processes();
    simulate(n_process, 1, 0);
    // simulate(n_process, 2, 0);
    // simulate(n_process, 3, 0);
    // simulate(n_process, 4, 0);
    // simulate(n_process, 5, 0);
    // simulate(n_process, 6, 6);
    // simulate(n_process, 7, 6);
    // simulate(n_process, 8, 0);
    // simulate(n_process, 9, 6);
    // simulate(n_process, 10, 6);
    // simulate(n_process, 11, 0);
    // simulate(n_process, 12, 0);
    return 0;
}