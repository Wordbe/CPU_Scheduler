#include "simulation.h"

void evalute(int n_process){
    puts("\n ===========================<evalution>===========================\n");
    float att = 0.0f; // average turnaround time
    float awt = 0.0f; // average waiting time
    float art = 0.0f; // average response time
    puts(" terminated   pid   turnaround time   waiting time   response time");
    puts(" -----------------------------------------------------------------");
    for (int i=0; i<n_process_terminatedQueue; i++){
        printf(" % 10d   % 3d   % 15d   % 12d   % 13d\n",
                i+1, terminatedQueue[i]->pid, terminatedQueue[i]->turnaround_time,
                terminatedQueue[i]->waiting_time, terminatedQueue[i]->response_time);
        att += terminatedQueue[i]->turnaround_time;
        awt += terminatedQueue[i]->waiting_time;
        art += terminatedQueue[i]->response_time;
    }
    puts("\n");
    float CPU_util = ((float)t - (float)idle_time) * 100 / t;
    float throughput = (float)n_process / (t - scheduling_start_time);
    printf(" Start time: %d, End time: %d\n", scheduling_start_time, t);
    printf(" CPU utilization: %.2f%% | throughput: %.2f(processes/time)\n", CPU_util, throughput);
    printf(" Average turnaround time: %.2f\n", att / n_process_terminatedQueue);
    printf(" Average waiting time: %.2f\n", awt / n_process_terminatedQueue);
    printf(" Average response time: %.2f\n", art / n_process_terminatedQueue);
}

void clear_terminatedQueue(){
    for (int i=0; i<n_process_terminatedQueue; i++){
        free(terminatedQueue[i]);
    }
    n_process_terminatedQueue = 0;
}

char *num2string(int num){
    switch(num){
        case 1: return "FCFS";
        case 2: return "NonPreemptive SJF";
        case 3: return "Preempiive SJF";
        case 4: return "Nonpreemptive Priority";
        case 5: return "Preemptive Priority";
        case 6: return "RR (Round Robin)";
        case 7: return "Lottery";
        case 8: return "HRN (Highest Response-Rate Next)";
        case 9: return "Multilevel Queue";
        case 10: return "Multilevel Feedback Queue";
        case 11: return "NonPreemptive LIF (Longest I/O First)";
        case 12: return "Preemptive LIF (Longest I/O First)";
        default: return NULL;
    }
}

void simulate(int n_process, int scheduling_type, int time_quantum){
    // Set job queue
    copy_from_originalQueue_to_jobQueue(n_process);
    maxHeap_sort(jobQueue, n_process_jobQueue, 0); // Sort jobqueue in order by arrival time.

    puts("\n---------------------<Job queue list>---------------------");
    print_all_processes(jobQueue, n_process_jobQueue);

    printf("\n-----------------------<%s schedule start>-----------------------\n", num2string(scheduling_type));
    t = jobQueue[0]->arrival_time; // earliest_arrival_time
    scheduling_start_time = t;
    for (int i=0; i<t; i++){ // print time for every time
        printf(" %-4d  +---------------- <-- Scheduling wait\n", i);
    }
    printf(" %-4d  +---------------- <-- Scheduling start\n", t);
    
    // Scheduling
    switch(scheduling_type){
        case 1:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 0);
            execute_FCFS(n_process);
            break;
        case 2:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 1);
            execute_SJF(n_process, false);
            break;
        case 3:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 1);
            execute_SJF(n_process, true);
            break;
        case 4:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 2);
            execute_Priority(n_process, false);
            break;
        case 5:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 2);
            execute_Priority(n_process, true);
            break;
        case 6:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 0);
            execute_RR(n_process, time_quantum);
            break;
        case 7:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 0);
            execute_Loterry(n_process, time_quantum);
            break;
        case 8:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 1);
            execute_HRN(n_process);
            break;
        case 9:{
            int priority_criteria = 3;
            if (job_schedule_MLQ(priority_criteria)){
                maxHeap_sort(foreground_readyQueue, n_process_fg_readyQueue, 0);
                maxHeap_sort(background_readyQueue, n_process_bg_readyQueue, 0);
            }
            execute_MLQ(n_process, time_quantum, priority_criteria);
            break;
        }
        case 10:{
            int priority_criteria = 3;
            if (job_schedule_MLQ(priority_criteria)){
                maxHeap_sort(foreground_readyQueue, n_process_fg_readyQueue, 0);
                maxHeap_sort(background_readyQueue, n_process_bg_readyQueue, 0);
            }
            // background readyqueue에서 대기시간이 aging condition보다 높으면 foreground로 upgrade
            int aging_condition = 50; 
            // foreground readyqueue에서 CPU burst가 demote condition보다 높으면 background로 downgrade
            int demote_condition = 30;
            execute_MLFQ(n_process, time_quantum, priority_criteria, aging_condition, demote_condition);
            break;
        }
        case 11:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 0);
            execute_LIF(n_process, false);
            break;
        case 12:
            if (job_schedule()) maxHeap_sort(readyQueue, n_process_readyQueue, 0);
            execute_LIF(n_process, true);
            break;
        default: puts("Choose among the available numbers");
    }
    
    // Evaluation
    evalute(n_process);

    // time and terminated queue initialize for next simulation
    t = 0;
    scheduling_start_time = 0;
    idle_time = 0;
    clear_terminatedQueue();
}