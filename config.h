#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {false, true} bool;

#define N_MAX_PROCESS 30

// Define process struct and pointer.
struct ProcessType{
    int pid;
    int cpu_burst;
    int io_burst;
    int arrival_time;    
    int priority;

    int cpu_burst_remaining;
    int io_burst_remaining;
    int io_burst_start;
    int actual_io_burst_start;
    int latest_arrivalTime_to_ReadyQueue;
    
    int turnaround_time;
    int waiting_time;
    int response_time;
};
typedef struct ProcessType* ProcessPtr;

void swap(ProcessPtr *a, ProcessPtr *b);
void maxHeap_sort(ProcessPtr *arr, int size, int priority_mode);

void copy_from_originalQueue_to_jobQueue(int n_process);

ProcessPtr pop_jobQueue();
void push_readyQueue(ProcessPtr p);
void push_nth_readyQueue(ProcessPtr p, int nth);
ProcessPtr pop_readyQueue();
ProcessPtr pop_nth_readyQueue(int nth);
void push_waitingQueue(ProcessPtr p);
ProcessPtr pop_waitingQueue();
void push_terminatedQueue(ProcessPtr p);

void push_fg_readyQueue(ProcessPtr p);
ProcessPtr pop_fg_readyQueue();
ProcessPtr pop_nth_fg_readyQueue(int nth);
void push_bg_readyQueue(ProcessPtr p);
ProcessPtr pop_bg_readyQueue();
ProcessPtr pop_nth_bg_readyQueue(int nth);

// Queue
extern ProcessPtr originalQueue[N_MAX_PROCESS];
extern ProcessPtr jobQueue[N_MAX_PROCESS];
extern ProcessPtr readyQueue[N_MAX_PROCESS];
extern ProcessPtr waitingQueue[N_MAX_PROCESS];
extern ProcessPtr terminatedQueue[N_MAX_PROCESS];

extern int n_process_originalQueue;
extern int n_process_jobQueue;
extern int n_process_readyQueue;
extern int n_process_waitingQueue;
extern int n_process_terminatedQueue;

extern ProcessPtr foreground_readyQueue[N_MAX_PROCESS];
extern int n_process_fg_readyQueue;
extern ProcessPtr background_readyQueue[N_MAX_PROCESS];
extern int n_process_bg_readyQueue;

#endif