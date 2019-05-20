#include "schedulings.h"
int t = 0; // scheduling time
int scheduling_start_time = 0;
int idle_time = 0;
ProcessPtr crp; // crp : Current Running Process

int prev_time; // prev_time of t
int next_time; // next_time of t



bool _check_io_terminated(){
    /*
        Check if there are I/O terminated processes in waiting queue;
        if terminated, move it from waiting queue to ready queue.
    */
    if (n_process_waitingQueue > 0){
        bool is_moved = false;
        int io_terminated_time = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;

        // I/O 가 termiated 되었으면
        if (t >= io_terminated_time){ 
            ProcessPtr tmp = pop_waitingQueue(); // waiting queue -> ready queue
            tmp->io_burst_remaining = 0;
            tmp->latest_arrivalTime_to_ReadyQueue = io_terminated_time;
            printf("(%-3d   |                 <-- pid(%d) I/O completed)\n", io_terminated_time, tmp->pid);
            push_readyQueue(tmp);
            is_moved = true;
            if (n_process_waitingQueue > 0){
                waitingQueue[0]->actual_io_burst_start = io_terminated_time;
            }
        }
        if (is_moved) return true;
        else return false;
    }
    return false;
}

// jobQueue -> readyQueue 프로세스 이동
bool job_schedule(){
    if (n_process_jobQueue > 0) {
        bool is_moved = false;
        int tmp_n_process_jobQueue = n_process_jobQueue;
        for (int i=0; i<tmp_n_process_jobQueue; i++) {
            int tmp_arrival_time = jobQueue[0]->arrival_time;
            if (t >= tmp_arrival_time){
                ProcessPtr tmp = pop_jobQueue(); // job queue -> ready queue
                printf("(%-3d   |                 <-- pid(%d) admitted)\n", tmp_arrival_time, tmp->pid);
                push_readyQueue(tmp);
                is_moved = true;
            }
        }
        if (is_moved) return true;
        else return false;
    }
    return false;
}


/* Get process for preemption
    - priority mode
    0 : arrival time
    1 : cpu burst remaining time
    2 : priority
*/
ProcessPtr _get_process_for_preemption(int next_time, ProcessPtr crp, int priority_mode){
    /*
        next_time 에서 ready queue에 들어있는 프로세스 중에 가장 작은 값(우선순위가 가장 높은 것)을 리턴.
        (prioirty mode에서 고른 값의 종류)
        next_time 까지 ready queue가 비어있으면 -1을 리턴.
    */
    ProcessPtr ps[N_MAX_PROCESS]; // 'processes' supposed to be moved to ready queue till next_time
    int ps_size = 0;
    for (int i=0; i<n_process_jobQueue; i++) {
        int tmp_arrival_time = jobQueue[i]->arrival_time;
        if (next_time >= tmp_arrival_time){
            ps[ps_size++] = jobQueue[i];
        }
    }
    if (n_process_waitingQueue > 0){
        int io_terminated_time = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;
        if (next_time >= io_terminated_time){
            waitingQueue[0]->latest_arrivalTime_to_ReadyQueue = io_terminated_time;
            ps[ps_size++] = waitingQueue[0];
        }
    }
    
    if (ps_size == 0) return NULL;
    else {
        switch(priority_mode){
            case 0:
                maxHeap_sort(ps, ps_size, priority_mode);
                return ps[0];
            case 1:
                maxHeap_sort(ps, ps_size, priority_mode);
                int crp_cpu_burst_remaining = crp->cpu_burst_remaining - (ps[0]->latest_arrivalTime_to_ReadyQueue - t);
                // running process 보다 (job queue나 waiting queue)에서 온 process가 cbrt가 더 작으면
                if (ps[0]->cpu_burst_remaining < crp_cpu_burst_remaining)
                    return ps[0];
                else return NULL;
            case 2:
                maxHeap_sort(ps, ps_size, priority_mode);
                if (ps[0]->priority < crp->priority)
                    return ps[0];
                else return NULL;
            default: return NULL;
        }
    }
}



void _idle_manage(int mode){
    puts("       | idle");
    prev_time = t;
    if (n_process_jobQueue > 0 && n_process_waitingQueue == 0){ // job queue만 차있으면
        t = jobQueue[0]->arrival_time;
        idle_time = t - prev_time;
        switch(mode){
            case 0: 
                job_schedule();
                break;
            case 1:
                if (job_schedule())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                break;
            case 2:
                if (job_schedule())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                break;
            default: break;
        }
        
    }
    else if (n_process_jobQueue == 0 && n_process_waitingQueue > 0){ // waiting queue만 차있으면
        t = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;
        idle_time = t - prev_time;
        switch(mode){
            case 0: 
                _check_io_terminated();
                break;
            case 1:
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                break;
            case 2:
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                break;
            default: break;
        }
    }
    else if (n_process_jobQueue > 0 && n_process_waitingQueue > 0){ // 둘 다 차있으면
        int time_from_job = jobQueue[0]->arrival_time;
        int time_from_waiting = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;
        int dif = time_from_job - time_from_waiting;
        if (dif > 0){ // waitig queue에서 프로세스 가져옴
            t = time_from_waiting;
            idle_time = t - prev_time;
            switch(mode){
            case 0: 
                _check_io_terminated();
                break;
            case 1:
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                break;
            case 2:
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                break;
            default: break;
        }
        }
        else{ // job queue에서 프로세스 가져옴
            t = time_from_job;
            idle_time = t - prev_time;
            switch(mode){
                case 0: 
                    job_schedule();
                    break;
                case 1:
                    if (job_schedule())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    break;
                case 2:
                    if (job_schedule())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                    break;
                default: break;
            }
        }
    }
}

void _random_io_occur(int probability){
    int random_num = rand() % probability;
    if (random_num == 1){
        crp->io_burst_remaining = rand() % 20 + 1;
        crp->io_burst_start = rand() % crp->cpu_burst_remaining;
    }
}

int _aging(){
    double max = 0.0;
    int arg_max = 0;
    for (int i=0; i<n_process_readyQueue; i++){
        // waiting time update
        int cpu_burst_consumed = readyQueue[i]->cpu_burst - readyQueue[i]->cpu_burst_remaining;
        readyQueue[i]->waiting_time = t - readyQueue[i]->arrival_time - cpu_burst_consumed;

        int WT = readyQueue[i]->waiting_time;
        int CRT = readyQueue[i]->cpu_burst_remaining;
        double DP = (double)(WT+CRT) / CRT;

        if (DP > max){
            max = DP;
            arg_max = i;
        }
    }
    return arg_max;
}


bool job_schedule_MLQ(int priority_criteria){
    if (n_process_jobQueue > 0) {
        bool is_moved = false;
        int tmp_n_process_jobQueue = n_process_jobQueue;
        for (int i=0; i<tmp_n_process_jobQueue; i++) {
            int tmp_arrival_time = jobQueue[0]->arrival_time;
            if (t >= tmp_arrival_time){
                ProcessPtr tmp = pop_jobQueue(); // job queue -> ready queue
                printf("(%-3d   |                 <-- pid(%d) admitted)\n", tmp_arrival_time, tmp->pid);
                if (tmp->priority <= priority_criteria){
                    push_fg_readyQueue(tmp);
                }
                else {
                    push_bg_readyQueue(tmp);
                }
                is_moved = true;
            }
        }
        if (is_moved) return true;
        else return false;
    }
    return false;
}

bool _check_io_terminated_MLQ(int priority_criteria){
    /*
        Check if there are I/O terminated processes in waiting queue;
        if terminated, move it from waiting queue to ready queue.
    */
    if (n_process_waitingQueue > 0){
        bool is_moved = false;
        int io_terminated_time = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;

        // I/O 가 termiated 되었으면
        if (t >= io_terminated_time){ 
            ProcessPtr tmp = pop_waitingQueue(); // waiting queue -> ready queue
            tmp->io_burst_remaining = 0;
            tmp->latest_arrivalTime_to_ReadyQueue = io_terminated_time;
            printf("(%-3d   |                 <-- pid(%d) I/O completed)\n", io_terminated_time, tmp->pid);
            
            if (tmp->priority <= priority_criteria){
                push_fg_readyQueue(tmp);
            }
            else {
                push_bg_readyQueue(tmp);
            }

            is_moved = true;
            if (n_process_waitingQueue > 0){ // 처음에 waiting queue에 2개이상 프로세스 있었을 때
                waitingQueue[0]->actual_io_burst_start = io_terminated_time;
            }
        }
        if (is_moved) return true;
        else return false;
    }
    return false;
}

void _idle_manage_MLQ(int priority_criteria){
    puts("       | idle");
    prev_time = t;
    if (n_process_jobQueue > 0 && n_process_waitingQueue == 0){ // job queue만 차있으면
        t = jobQueue[0]->arrival_time;
        idle_time = t - prev_time;
        job_schedule_MLQ(priority_criteria);
    }
    else if (n_process_jobQueue == 0 && n_process_waitingQueue > 0){ // waiting queue만 차있으면
        t = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;
        idle_time = t - prev_time;
        _check_io_terminated_MLQ(priority_criteria);
    }
    else if (n_process_jobQueue > 0 && n_process_waitingQueue > 0){ // 둘 다 차있으면
        int time_from_job = jobQueue[0]->arrival_time;
        int time_from_waiting = waitingQueue[0]->actual_io_burst_start + waitingQueue[0]->io_burst_remaining;
        int dif = time_from_job - time_from_waiting;
        if (dif > 0){ // waitig queue에서 프로세스 가져옴
            t = time_from_waiting;
            idle_time = t - prev_time;
            _check_io_terminated_MLQ(priority_criteria);
        }
        else{ // job queue에서 프로세스 가져옴
            t = time_from_job;
            idle_time = t - prev_time;
            job_schedule_MLQ(priority_criteria);
        }
    }
}

void _aging_from_backgroundQueue_MLFQ(int aging_condition){
    double max = 0.0;
    int arg_max = 0;
    for (int i=0; i<n_process_bg_readyQueue; i++){
        // waiting time update
        int WT = background_readyQueue[i]->waiting_time;
        if (WT > aging_condition){
            push_fg_readyQueue(pop_nth_bg_readyQueue(i));
        }
    }
}

void _demote_from_foregroundQueue_MLFQ(int demote_condition){
    double max = 0.0;
    int arg_max = 0;
    for (int i=0; i<n_process_fg_readyQueue; i++){
        // waiting time update
        int cpu_burst_consumed = foreground_readyQueue[i]->cpu_burst 
                                - foreground_readyQueue[i]->cpu_burst_remaining;
        if (cpu_burst_consumed > demote_condition){
            push_bg_readyQueue(pop_nth_fg_readyQueue(i));
        }
    }
}



void _sort_by_IO_burst(){
    ProcessPtr temp[N_MAX_PROCESS];
    int temp_size = 0;
    for (int i=0; i<n_process_readyQueue; i++){
        if (readyQueue[i]->io_burst_remaining > 0){
            temp[temp_size++] = pop_nth_readyQueue(i);
            i--;
        }
    }
    maxHeap_sort(temp, temp_size, 3); // I/O 작은 것부터 앞으로 정렬
    for (int i=0; i<temp_size; i++){
        push_nth_readyQueue(temp[i], 0);
    }
}
// =============================================================================




// Non-Preemptive
void execute_FCFS(int n_process){
    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
            && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }

        // 2. Idle state
        if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
            _idle_manage(0);
        }
        // 3. running state
        else {
            // CPU scheduling
            crp = pop_readyQueue();

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                t += crp->io_burst_start;
                crp->actual_io_burst_start = t;
                job_schedule();
                _check_io_terminated();

                printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                crp->cpu_burst_remaining -= crp->io_burst_start;
                push_waitingQueue(crp); // crp: running state -> waiting queue
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                // current running process terminated
                t += crp->cpu_burst_remaining;
                job_schedule();
                _check_io_terminated();

                crp->cpu_burst_remaining = 0;
                crp->turnaround_time = t - crp->arrival_time;
                crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                push_terminatedQueue(crp); // crp: running state -> terminated queue
            }
        }
    }
}


// Non-Preemptive / Preemptive
void execute_SJF(int n_process, bool is_preemptive){

    // Preemptive SJF
    if (is_preemptive){
        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(1);
            }
            // 3. running state
            else {
                // cpu scheduling (ready queue -> running state)
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    if (readyQueue)
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->io_burst_start;
                    // smallest =  highest priority process
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 1);

                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;
                        push_readyQueue(crp);
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        
                        printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    }
                    else {
                        // time update
                        t += crp->io_burst_start;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        
                        crp->actual_io_burst_start = t;
                        printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                        
                        crp->cpu_burst_remaining -= crp->io_burst_start;
                        push_waitingQueue(crp);
                    }
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                    
                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->cpu_burst_remaining;
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 1);

                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;

                        if (crp->cpu_burst_remaining == 0){ // running -> terminated
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                            crp->cpu_burst_remaining = 0;
                            crp->turnaround_time = t - crp->arrival_time;
                            crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                            printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                            push_terminatedQueue(crp);
                        }
                        else { // running -> ready
                            push_readyQueue(crp);
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                            printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                        }
                    }
                    else{
                        // time update
                        t += crp->cpu_burst_remaining;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                        crp->cpu_burst_remaining = 0;
                        crp->turnaround_time = t - crp->arrival_time;
                        crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                        printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                        push_terminatedQueue(crp);
                    }
                }
            }
        }
    }

    // Non-preemptive SJF
    else {

        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(1);
            }
            // 3. running state
            else {
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                    
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue
                }
            }
        }
    }
}


// Non-Preemptive / Preemptive
void execute_Priority(int n_process, bool is_preemptive){

    // Preemptive SJF
    if (is_preemptive){
        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(2);
            }
            // 3. running state
            else {
                // cpu scheduling (ready queue -> running state)
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    if (readyQueue)
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->io_burst_start;
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 2);

                    // Preemption occur, running -> ready
                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;
                        push_readyQueue(crp);
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                        
                        printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    }
                    // Preemption does not occur, running -> waiting
                    else {
                        // time update
                        t += crp->io_burst_start;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                        
                        crp->actual_io_burst_start = t;
                        printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                        
                        crp->cpu_burst_remaining -= crp->io_burst_start;
                        push_waitingQueue(crp);
                    }
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->cpu_burst_remaining;
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 2);

                    // Preemption ocurrs, running -> ready
                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;
                        
                        if (crp->cpu_burst_remaining == 0){ // running -> terminated
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 2);

                            crp->cpu_burst_remaining = 0;
                            crp->turnaround_time = t - crp->arrival_time;
                            crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                            printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                            push_terminatedQueue(crp);
                        }
                        else { // running -> ready
                            push_readyQueue(crp);
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 2);

                            printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                        }
                    }
                    // Preemption does not occur, running -> terminated
                    else{
                        // time update
                        t += crp->cpu_burst_remaining;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 2);

                        crp->cpu_burst_remaining = 0;
                        crp->turnaround_time = t - crp->arrival_time;
                        crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                        printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                        push_terminatedQueue(crp);
                    }
                }
            }
        }
    }

    // Non-preemptive SJF
    else {

        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(2);
            }
            // 3. running state
            else {
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                    
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 2);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 2);

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue
                }
            }
        }
    }
}


// Preemptive (FCFS + time quantum)
void execute_RR(int n_process, int time_quantum){

    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
            && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }
        // 2. Idle state
        // ready queue가 비었을 때
        if (n_process_readyQueue <= 0){ 
            _idle_manage(0);
        }
        // 3. running state
        else {
            crp = pop_readyQueue();

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                if (crp->io_burst_start > time_quantum) {
                    crp->io_burst_start -= time_quantum;
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule();
                    _check_io_terminated();
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_readyQueue(crp);
                }
                else {
                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    job_schedule();
                    _check_io_terminated();
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                
                if (crp->cpu_burst_remaining > time_quantum) {
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule();
                    _check_io_terminated();
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_readyQueue(crp);
                }
                else {
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    job_schedule();
                    _check_io_terminated();

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue 
                }
            }
        }
    }
}


// Preemptive (Lottery + RR)
void execute_Loterry(int n_process, int time_quantum){

    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
            && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }
        // 2. Idle state
        // ready queue가 비었을 때
        if (n_process_readyQueue <= 0){ 
            _idle_manage(0);
        }
        // 3. running state
        else {

            // Give tickets to processes in ready queue
            int sum = 0;
            for (int i=0; i<n_process_readyQueue; i++) sum += readyQueue[i]->priority;

            int lottery[sum];
            int idx = 0;
            for (int i=0; i<n_process_readyQueue; i++){
                for (int j=0; j<readyQueue[i]->priority; j++){
                    lottery[idx++] = readyQueue[i]->priority;
                }
            }

            // 추첨
            srand((unsigned int)time(NULL));
            int win_num = rand() % sum;
            for (int i=0; i<n_process_readyQueue; i++){
                if (readyQueue[i]->priority == lottery[win_num]){
                    crp = pop_nth_readyQueue(i);
                    break;
                }
            }

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                if (crp->io_burst_start > time_quantum) {
                    crp->io_burst_start -= time_quantum;
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule();
                    _check_io_terminated();
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_readyQueue(crp);
                }
                else {
                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    job_schedule();
                    _check_io_terminated();
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                
                if (crp->cpu_burst_remaining > time_quantum) {
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule();
                    _check_io_terminated();
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_readyQueue(crp);
                }
                else {
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    job_schedule();
                    _check_io_terminated();

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue 
                }
            }
        }
    }
}


// Non-Preemptive (Highest Response-Rate Next Scheduling)
// higher prioirty = higher value of "(대기시간 + 실행시간) / 실행시간"
void execute_HRN(int n_process){

    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
            && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }
        // 2. Idle state
        if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
            _idle_manage(1);
        }
        // 3. running state
        else {
            
            // Aging (for saving starvation)
            // highest priority (max Dynamic Priority)
            int max_DP_idx = _aging();
            
            crp = pop_nth_readyQueue(max_DP_idx);
            


            if (crp->io_burst_remaining == 0){
                _random_io_occur(10); // 1/10 확률로 io 발생
            }

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                // response time update
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                t += crp->io_burst_start;

                crp->actual_io_burst_start = t;
                if (job_schedule()) 
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                
                printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                
                crp->cpu_burst_remaining -= crp->io_burst_start;
                push_waitingQueue(crp); // crp: running state -> waiting queue
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                
                // current running process terminated
                t += crp->cpu_burst_remaining;
                if (job_schedule()) 
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                if (_check_io_terminated())
                    maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                crp->cpu_burst_remaining = 0;
                crp->turnaround_time = t - crp->arrival_time;
                crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                push_terminatedQueue(crp); // crp: running state -> terminated queue
            }
        }
    }
}



// Preemptive
// MLQ : Multi Level Queue Scheduling
void execute_MLQ(int n_process, int time_quantum, int priority_criteria){
    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_fg_readyQueue == 0 && n_process_bg_readyQueue == 0 && 
            n_process_waitingQueue == 0 && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }
        // 2. Idle state
        if (n_process_fg_readyQueue <= 0 && n_process_bg_readyQueue <= 0){ // 모든 ready queue 비었을 때
            _idle_manage_MLQ(priority_criteria);
        }
        // 3.1 background (FCFS)에서 running
        else if (n_process_fg_readyQueue == 0 && n_process_bg_readyQueue > 0){ // background ready queue만 차있을 때
            crp = pop_bg_readyQueue();

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                t += crp->io_burst_start;
                crp->actual_io_burst_start = t;
                job_schedule_MLQ(priority_criteria);
                _check_io_terminated_MLQ(priority_criteria);

                printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                crp->cpu_burst_remaining -= crp->io_burst_start;
                push_waitingQueue(crp); // crp: running state -> waiting queue
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                // current running process terminated
                t += crp->cpu_burst_remaining;
                job_schedule_MLQ(priority_criteria);
                _check_io_terminated_MLQ(priority_criteria);

                crp->cpu_burst_remaining = 0;
                crp->turnaround_time = t - crp->arrival_time;
                crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                push_terminatedQueue(crp); // crp: running state -> terminated queue
            }
        }
        // 3.2 foreground (Round Robin)에서 running
        else {
            crp = pop_fg_readyQueue();

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                if (crp->io_burst_start > time_quantum) {
                    crp->io_burst_start -= time_quantum;
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_fg_readyQueue(crp);
                }
                else {
                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                
                if (crp->cpu_burst_remaining > time_quantum) {
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_fg_readyQueue(crp);
                }
                else {
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue 
                }
            }
        }
    }

}



// Preemptive
// MLFQ : Multi Level Feedback Queue Scheduling
void execute_MLFQ(int n_process, int time_quantum, int priority_criteria, int aging_condition, int demote_condition){
    while(1){
        // 1. Termination
        if (n_process_jobQueue == 0 && n_process_fg_readyQueue == 0 && n_process_bg_readyQueue == 0 && 
            n_process_waitingQueue == 0 && n_process_terminatedQueue == n_process){
            puts("\n all processes terminated.");
            break;
        }
        // 2. Idle state
        if (n_process_fg_readyQueue <= 0 && n_process_bg_readyQueue <= 0){ // 모든 ready queue 비었을 때
            _idle_manage_MLQ(priority_criteria);
        }
        // 3.1 background (FCFS)에서 running
        else if (n_process_fg_readyQueue == 0 && n_process_bg_readyQueue > 0){ // background ready queue만 차있을 때
            
            // backgournd queue에서 대기시간이 aging_condtion보다 높은 프로세스는 foreground로 aging한다.
            _aging_from_backgroundQueue_MLFQ(aging_condition);
            if (n_process_bg_readyQueue > 0)
                crp = pop_bg_readyQueue();
            else continue;

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                t += crp->io_burst_start;
                crp->actual_io_burst_start = t;
                job_schedule_MLQ(priority_criteria);
                _check_io_terminated_MLQ(priority_criteria);

                printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                crp->cpu_burst_remaining -= crp->io_burst_start;
                push_waitingQueue(crp); // crp: running state -> waiting queue
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                // current running process terminated
                t += crp->cpu_burst_remaining;
                job_schedule_MLQ(priority_criteria);
                _check_io_terminated_MLQ(priority_criteria);

                crp->cpu_burst_remaining = 0;
                crp->turnaround_time = t - crp->arrival_time;
                crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                push_terminatedQueue(crp); // crp: running state -> terminated queue
            }
        }
        // 3.2 foreground (Round Robin)에서 running
        else {

            // foreground queue에서 소모한 CPU burst가 demote_contion보다 높으면 background로 demote한다. 
            _demote_from_foregroundQueue_MLFQ(demote_condition);
            if (n_process_fg_readyQueue > 0){
                crp = pop_fg_readyQueue();
            }
            else continue;

            // 3.1 running -> waiting
            if (crp->io_burst_remaining > 0){
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                if (crp->io_burst_start > time_quantum) {
                    crp->io_burst_start -= time_quantum;
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_fg_readyQueue(crp);
                }
                else {
                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                
            }
            // 3.2 running -> terminated
            else{
                printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                printf("       | pid(%d) running\n", crp->pid);
                if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                
                if (crp->cpu_burst_remaining > time_quantum) {
                    crp->cpu_burst_remaining -= time_quantum;

                    t += time_quantum;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);
                    printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    push_fg_readyQueue(crp);
                }
                else {
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    job_schedule_MLQ(priority_criteria);
                    _check_io_terminated_MLQ(priority_criteria);

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue 
                }
            }
        }
    }

}


// Non-preemptive / preemptive
// LIF : Longest I/O burst First Scheduling
// 스스로 만들어 본 스케쥴링 기법, SJF에 LIF 옵션을 추가했다.
void execute_LIF(int n_process, bool is_preemptive){

    // Preemptive SJF
    if (is_preemptive){
        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(0);
            }
            // 3. running state
            else {
                // cpu scheduling (ready queue -> running state)
                // preemptive SJF로 진행하다가, IO가 가장 긴 것이 있으면 그것을 먼저한다.
                _sort_by_IO_burst();
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    if (readyQueue)
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->io_burst_start;
                    // smallest =  highest priority process
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 1);

                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;
                        push_readyQueue(crp);
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        
                        printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                    }
                    else {
                        // time update
                        t += crp->io_burst_start;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        
                        crp->actual_io_burst_start = t;
                        printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                        
                        crp->cpu_burst_remaining -= crp->io_burst_start;
                        push_waitingQueue(crp);
                    }
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                    
                    // When preemption occurs, the current running process interrupted -> ready queue
                    next_time = t + crp->cpu_burst_remaining;
                    ProcessPtr smallest = _get_process_for_preemption(next_time, crp, 1);

                    if (smallest != NULL){
                        crp->cpu_burst_remaining -= (smallest->latest_arrivalTime_to_ReadyQueue - t);
                        t = smallest->latest_arrivalTime_to_ReadyQueue;

                        if (crp->cpu_burst_remaining == 0){ // running -> terminated
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                            crp->cpu_burst_remaining = 0;
                            crp->turnaround_time = t - crp->arrival_time;
                            crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                            printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                            push_terminatedQueue(crp);
                        }
                        else { // running -> ready
                            push_readyQueue(crp);
                            if (job_schedule()) 
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                            if (_check_io_terminated())
                                maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                            printf(" %-4d  +---------------- <-- pid(%d) interrupted\n", t, crp->pid);
                        }
                    }
                    else{
                        // time update
                        t += crp->cpu_burst_remaining;
                        if (job_schedule()) 
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                        if (_check_io_terminated())
                            maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                        crp->cpu_burst_remaining = 0;
                        crp->turnaround_time = t - crp->arrival_time;
                        crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                        printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                        push_terminatedQueue(crp);
                    }
                }
            }
        }
    }

    // Non-preemptive SJF
    else {

        print_all_processes(readyQueue, n_process_readyQueue);

        while(1){
            // 1. Termination
            if (n_process_jobQueue == 0 && n_process_readyQueue == 0 && n_process_waitingQueue == 0
                && n_process_terminatedQueue == n_process){
                puts("\n all processes terminated.");
                break;
            }
            // 2. Idle state
            if (n_process_readyQueue <= 0){ // ready queue가 비었을 때
                _idle_manage(1);
            }
            // 3. running state
            else {
                // I/O가 가장 긴 것이 있으면, ready queue가장 앞으로 정렬
                _sort_by_IO_burst();
                crp = pop_readyQueue();

                // 3.1 running -> waiting
                if (crp->io_burst_remaining > 0){
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;

                    t += crp->io_burst_start;
                    crp->actual_io_burst_start = t;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    
                    printf(" %-4d  +---------------- <-- pid(%d) I/O start\n", t, crp->pid);
                    
                    crp->cpu_burst_remaining -= crp->io_burst_start;
                    push_waitingQueue(crp); // crp: running state -> waiting queue
                }
                // 3.2 running -> terminated
                else{
                    printf(" %-4d  +---------------- <-- pid(%d) start\n", t, crp->pid);
                    printf("       | pid(%d) running\n", crp->pid);
                    if (crp->response_time == -1) crp->response_time = t - crp->arrival_time;
                    
                    // current running process terminated
                    t += crp->cpu_burst_remaining;
                    if (job_schedule()) 
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);
                    if (_check_io_terminated())
                        maxHeap_sort(readyQueue, n_process_readyQueue, 1);

                    crp->cpu_burst_remaining = 0;
                    crp->turnaround_time = t - crp->arrival_time;
                    crp->waiting_time = crp->turnaround_time - crp->cpu_burst;
                    printf(" %-4d  +---------------- <-- pid(%d) terminated\n", t, crp->pid);
                    push_terminatedQueue(crp); // crp: running state -> terminated queue
                }
            }
        }
    }
}