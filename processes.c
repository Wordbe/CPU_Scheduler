#include "processes.h"

// 중복 없이 1~n에서 랜덤으로 뽑아 array에 넣음(srand((unsigned int)time(NULL)); 호출하고 부를 것)
void _random_without_overlap(int *arr, int arr_size, int n_max){
    for (int i=0; i<arr_size; i++) arr[i] = 0;

    int cnt = 0;
    while(cnt < arr_size){
        bool is_same = false;
        int r = rand() % n_max + 1;

        for (int i=0; i<cnt; i++){
            if (arr[i] == r){
                is_same = true;
                break;
            }
        }
        if (!is_same){
            arr[cnt] = r;
            cnt++;
        }
    }
}

// Create process
ProcessPtr create_one_process(int pid, int cpu_burst, int io_burst, int arrival_time, int priority){
    ProcessPtr p = (ProcessPtr)malloc(sizeof(struct ProcessType));
    p->pid = pid;
    p->cpu_burst = cpu_burst;
    p->io_burst = io_burst;
    p->arrival_time = arrival_time;
    p->priority = priority;

    p->cpu_burst_remaining = cpu_burst;
    p->io_burst_remaining = io_burst;
    p->actual_io_burst_start = arrival_time;
    p->io_burst_start = 0;
    if (io_burst > 0){
        p->io_burst_start = rand() % (cpu_burst - 1) + 1;
    }
    p->latest_arrivalTime_to_ReadyQueue = arrival_time;

    p->turnaround_time = 0;
    p->waiting_time = 0;
    p->response_time = -1;

    return p;
}

void print_all_processes(ProcessPtr *p, int n_process){
    for (int i=0; i<n_process; i++){
        printf("process(%d): pid(%d), cpu_burst(%d), io_burst(%d), arrival_time(%d), priority(%d), io_burst_start(%d)\n", 
            i+1, p[i]->pid, p[i]->cpu_burst, p[i]->io_burst, p[i]->arrival_time, p[i]->priority,
            p[i]->io_burst_start);
    }
}

void print_process(ProcessPtr p){
    printf("pid(%d), cbt(%d), ibt(%d), at(%d), priority(%d), remain_cbt(%d), remain_iot(%d), ibt_start(%d)\n",
        p->pid, p->cpu_burst, p->io_burst, p->arrival_time, p->priority,
        p->cpu_burst_remaining, p->io_burst_remaining, p->io_burst_start);
}

// process를 만들어 jobQueue에 넣음.
void create_processes(int n_process, int n_io){
    srand((unsigned int)time(NULL));

    // for priority values without overlap
    int random_priorities[n_process];
    _random_without_overlap(random_priorities, n_process, n_process);

    // 랜덤으로 io event를 가지는 프로세스를 정하고, I/O burst time을 정해준다.
    int random_io_bursts[n_process];
    for (int i=0; i<n_process; i++) random_io_bursts[i] = 0;
    int tmp[n_io];
    _random_without_overlap(tmp, n_io, n_process);

    for (int i=0; i<n_io; i++){
        int process_with_io_dix = tmp[i] - 1;
        random_io_bursts[process_with_io_dix] = rand() % 100 + 1;
    }

    // create processes
    for (int i=0; i<n_process; i++){
        int pid = i + 1;
        int cpu_burst = rand() % 20 + 1;
        int io_burst = random_io_bursts[i];
        int arrival_time = rand() % 10;
        int priority = random_priorities[i];
        originalQueue[i] = create_one_process(pid, cpu_burst, io_burst, arrival_time, priority);
    }
    n_process_originalQueue = n_process;

    puts("\n---------------------<Original queue list>---------------------");
    print_all_processes(originalQueue, n_process_originalQueue);
}

// --------------------------------------------------------------------------------------
void create_test_processes(){
    // process 5개, io 2개
    int n_process = 9;
    //                      pid   cbt   ibt  arrival prior io_start
    int test_set[9][6] = {  {1,     5,    7,       6,    9,       2},
                            {2,     6,    3,       6,    8,       1},
                            {3,     2,    2,       4,    5,       1},
                            {4,     6,   15,       4,    3,       4},
                            {5,    12,    0,       4,    6,       0},
                            {6,    40,    0,       5,    1,       0},
                            {7,    12,    9,       6,    2,       3},
                            {8,    11,    0,       7,    7,       0},
                            {9,     3,    0,       8,    4,       0},
                         };
    // int test_set[9][6] = {  {1,     5,    7,       6,    9,       2},
    //                         {2,     6,    3,       6,    8,       1},
    //                         {3,     2,    2,       3,    5,       1},
    //                         {4,     6,   15,       3,    3,       4},
    //                         {5,    12,    0,       2,    6,       0},
    //                         {6,    40,    0,       4,    1,       0},
    //                         {7,    12,    9,       3,    2,       3},
    //                         {8,    11,    0,       1,    7,       0},
    //                         {9,     3,    0,       4,    4,       0},
    //                      };
    // int test_set[9][6] = {  {1,     5,    0,       6,    9,       0},
    //                         {2,     6,    0,       6,    8,       0},
    //                         {3,     2,    0,       3,    5,       0},
    //                         {4,     6,   15,       3,    3,       4},
    //                         {5,    12,    0,       2,    6,       0},
    //                         {6,    20,    4,       4,    1,       1},
    //                         {7,    12,    9,       3,    2,       3},
    //                         {8,    11,    0,       1,    7,       0},
    //                         {9,     3,    0,       4,    4,       0},
    //                      };

    // int test_set[5][6] = {  {1,     5,   100,       3,    1,       1},
    //                         {2,     5,    3,       3,    2,       2},
    //                         {3,     8,    0,       4,    3,       0},
    //                         {4,    10,    0,       5,    4,       0},
    //                         {5,     1,    0,       7,    5,       0},
    //                      };
    // int test_set[5][6] = {  {2, 5, 3, 3, 2, 2},
    //                         {1, 5, 2, 3, 1, 1},
    //                         {3, 8, 0, 4, 3, 0},
    //                         {4, 10, 0, 5, 4, 0},
    //                         {5, 3, 0, 8, 5, 0},
    //                      };
    // create processes
    for (int i=0; i<n_process; i++){
        ProcessPtr p = (ProcessPtr)malloc(sizeof(struct ProcessType));
        p->pid = test_set[i][0];
        p->cpu_burst = test_set[i][1];
        p->io_burst = test_set[i][2];
        p->arrival_time = test_set[i][3];
        p->priority = test_set[i][4];
        
        p->cpu_burst_remaining = test_set[i][1];
        p->io_burst_remaining = test_set[i][2];
        p->io_burst_start = test_set[i][5];
        p->latest_arrivalTime_to_ReadyQueue = test_set[i][3];

        p->actual_io_burst_start = test_set[i][3];
        p->turnaround_time = 0;
        p->waiting_time = 0;
        p->response_time = -1;

        originalQueue[i] = p;
    }
    n_process_originalQueue = n_process;

    puts("\n---------------------<Original queue list>---------------------");
    print_all_processes(originalQueue, n_process_originalQueue);
}