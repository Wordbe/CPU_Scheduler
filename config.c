#include "config.h"

// Heap sort (using max heap)
// --------------------------------------------------------------------------------------
void swap(ProcessPtr *a, ProcessPtr *b){ProcessPtr t = *a; *a = *b; *b = t;}

// reheap down (가장 작은 노드를 root로 올려줌, 큰 노드는 재귀적으로 밑으로 내려감)
void _heapify(ProcessPtr *arr, int size, int idx, int priority_mode){
    int largest = idx; // root
    int left = 2*idx + 1;
    int right = 2*idx + 2;

    switch(priority_mode){
        case 0:
            if (left < size &&
                arr[left]->arrival_time > arr[largest]->arrival_time)
                largest = left;
            if (right < size &&
                arr[right]->arrival_time > arr[largest]->arrival_time)
                largest = right;
            if (largest != idx){
                swap(&arr[largest], &arr[idx]);
                _heapify(arr, size, largest, priority_mode);
            }
            break;
        case 1:
            if (left < size &&
                arr[left]->cpu_burst_remaining > arr[largest]->cpu_burst_remaining)
                largest = left;
            if (right < size &&
                arr[right]->cpu_burst_remaining > arr[largest]->cpu_burst_remaining)
                largest = right;
            if (largest != idx){
                swap(&arr[largest], &arr[idx]);
                _heapify(arr, size, largest, priority_mode);
            }
            break;
        case 2:
            if (left < size &&
                arr[left]->priority > arr[largest]->priority)
                largest = left;
            if (right < size &&
                arr[right]->priority > arr[largest]->priority)
                largest = right;
            if (largest != idx){
                swap(&arr[largest], &arr[idx]);
                _heapify(arr, size, largest, priority_mode);
            }
            break;
         case 3:
            if (left < size &&
                arr[left]->io_burst_remaining > arr[largest]->io_burst_remaining)
                largest = left;
            if (right < size &&
                arr[right]->io_burst_remaining > arr[largest]->io_burst_remaining)
                largest = right;
            if (largest != idx){
                swap(&arr[largest], &arr[idx]);
                _heapify(arr, size, largest, priority_mode);
            }
            break;
        default: break;
    }
}

/* Max heap sort
    - priority mode
    0 : arrival time
    1 : cpu burst remaining time
    2 : priority
    3 : I/O burst remaining time
*/
void maxHeap_sort(ProcessPtr *arr, int size, int priority_mode){
    if (size < 1)
        return;
    else{
        if (priority_mode < 0 || priority_mode > 3) {
            puts("error: priority mode should be from 0 to 3");
        }
        else {
            // Build heap
            // 오른쪽 아래에서부터 왼쪽으로, 그 다음 위로, root까지 heapify 한다. 
            int parent_of_lastNode = (size -2)/2;
            for (int i=parent_of_lastNode; i>=0; i--)
                _heapify(arr, size, i, priority_mode);
            
            // Sort
            while(size > 1){
                swap(&arr[0], &arr[size - 1]);
                size--;
                _heapify(arr, size, 0, priority_mode);
            }
        }
    }
}
// --------------------------------------------------------------------------------------


// Queue
// --------------------------------------------------------------------------------------
ProcessPtr originalQueue[N_MAX_PROCESS];
int n_process_originalQueue = 0;
ProcessPtr jobQueue[N_MAX_PROCESS];
int n_process_jobQueue = 0;
ProcessPtr readyQueue[N_MAX_PROCESS];
int n_process_readyQueue = 0;
ProcessPtr waitingQueue[N_MAX_PROCESS];
int n_process_waitingQueue = 0;
ProcessPtr terminatedQueue[N_MAX_PROCESS];
int n_process_terminatedQueue = 0;

ProcessPtr foreground_readyQueue[N_MAX_PROCESS];
int n_process_fg_readyQueue = 0;
ProcessPtr background_readyQueue[N_MAX_PROCESS];
int n_process_bg_readyQueue = 0;

void copy_from_originalQueue_to_jobQueue(int n_process){

    for (int i=0; i<n_process; i++){
        ProcessPtr p = (ProcessPtr)malloc(sizeof(struct ProcessType));
        p->pid = originalQueue[i]->pid;
        p->cpu_burst = originalQueue[i]->cpu_burst;
        p->io_burst = originalQueue[i]->io_burst;
        p->arrival_time = originalQueue[i]->arrival_time;
        p->priority = originalQueue[i]->priority;

        p->cpu_burst_remaining = originalQueue[i]->cpu_burst_remaining;
        p->io_burst_remaining = originalQueue[i]->io_burst_remaining;
        p->io_burst_start = originalQueue[i]->io_burst_start;
        p->actual_io_burst_start = originalQueue[i]->actual_io_burst_start;
        p->latest_arrivalTime_to_ReadyQueue = originalQueue[i]->latest_arrivalTime_to_ReadyQueue;

        p->turnaround_time = originalQueue[i]->turnaround_time;
        p->waiting_time = originalQueue[i]->waiting_time;
        p->response_time = originalQueue[i]->response_time;

        jobQueue[i] = p;
    }
    n_process_jobQueue = n_process_originalQueue;
}


// O(n)
ProcessPtr pop_jobQueue(){
    if (n_process_jobQueue > 0){
        ProcessPtr poped = jobQueue[0];
        n_process_jobQueue--;

        // 배열 원소를 한 칸씩 앞으로 복사 후, 마지막 원소 초기화
        for (int i=0; i<n_process_jobQueue; i++){ 
            jobQueue[i] = jobQueue[i + 1];
        }
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        jobQueue[n_process_jobQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: job queue is empty.");
        return NULL;
    }
}


// O(1)
void push_readyQueue(ProcessPtr p){
    if (n_process_readyQueue < N_MAX_PROCESS){
        readyQueue[n_process_readyQueue++] = p; // rear에 process 삽입
    }
    else {
        puts("error: ready queue is full.");
    }
}

// O(1)
void push_nth_readyQueue(ProcessPtr p, int nth){
    if (n_process_readyQueue < N_MAX_PROCESS){
        n_process_readyQueue++;
        for (int i=n_process_readyQueue-1; i>nth; i--){
            readyQueue[i] = readyQueue[i-1];
        }
        readyQueue[nth] = p;
    }
    else {
        puts("error: ready queue is full.");
    }
}

// O(n)
ProcessPtr pop_readyQueue(){
    if (n_process_readyQueue > 0){
        ProcessPtr poped = readyQueue[0];
        n_process_readyQueue--;

        // 배열 원소를 한 칸씩 앞으로 복사 후, 마지막 원소 초기화
        for (int i=0; i<n_process_readyQueue; i++){ 
            readyQueue[i] = readyQueue[i + 1];
        }
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        readyQueue[n_process_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}

// about O(n)
ProcessPtr pop_nth_readyQueue(int nth){
    if (n_process_readyQueue > 0){

        ProcessPtr poped = readyQueue[nth];
        n_process_readyQueue--;

        for (int i=nth; i<n_process_readyQueue; i++){ 
            readyQueue[i] = readyQueue[i + 1];
        }
        
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        readyQueue[n_process_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}



// O(1)
void push_waitingQueue(ProcessPtr p){
    if (n_process_waitingQueue < N_MAX_PROCESS){
        waitingQueue[n_process_waitingQueue++] = p; // 배열 맨 마지막에 p 삽입
    }
    else {
        puts("error: waiting queue is full.");
    }
}

// O(n)
ProcessPtr pop_waitingQueue(){
    if (n_process_waitingQueue > 0){
        ProcessPtr poped = waitingQueue[0]; // front process를 제거
        n_process_waitingQueue--;

        // 배열 원소를 한 칸씩 앞으로 복사 후, 마지막 원소 초기화
        for (int i=0; i<n_process_waitingQueue; i++){ 
            waitingQueue[i] = waitingQueue[i + 1];
        }
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        waitingQueue[n_process_waitingQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: waiting queue is empty.");
        return NULL;
    }
}



// O(1)
void push_terminatedQueue(ProcessPtr p){
    if (n_process_terminatedQueue < N_MAX_PROCESS){
        terminatedQueue[n_process_terminatedQueue++] = p; // 배열 맨 마지막에 p 삽입
    }
    else {
        puts("error: terminated queue is full.");
    }
}


// O(1)
void push_fg_readyQueue(ProcessPtr p){
    if (n_process_fg_readyQueue < N_MAX_PROCESS){
        foreground_readyQueue[n_process_fg_readyQueue++] = p; // rear에 process 삽입
    }
    else {
        puts("error: ready queue is full.");
    }
}

// O(n)
ProcessPtr pop_fg_readyQueue(){
    if (n_process_fg_readyQueue > 0){
        ProcessPtr poped = foreground_readyQueue[0];
        n_process_fg_readyQueue--;

        // 배열 원소를 한 칸씩 앞으로 복사 후, 마지막 원소 초기화
        for (int i=0; i<n_process_fg_readyQueue; i++){ 
            foreground_readyQueue[i] = foreground_readyQueue[i + 1];
        }
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        foreground_readyQueue[n_process_fg_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}

// about O(n)
ProcessPtr pop_nth_fg_readyQueue(int nth){
    if (n_process_fg_readyQueue > 0){
        ProcessPtr poped = foreground_readyQueue[nth];
        n_process_fg_readyQueue--;

        for (int i=nth; i<n_process_fg_readyQueue; i++){ 
            foreground_readyQueue[i] = foreground_readyQueue[i + 1];
        }
        
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        foreground_readyQueue[n_process_fg_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}

// O(1)
void push_bg_readyQueue(ProcessPtr p){
    if (n_process_bg_readyQueue < N_MAX_PROCESS){
        background_readyQueue[n_process_bg_readyQueue++] = p; // rear에 process 삽입
    }
    else {
        puts("error: ready queue is full.");
    }
}

// O(n)
ProcessPtr pop_bg_readyQueue(){
    if (n_process_bg_readyQueue > 0){
        ProcessPtr poped = background_readyQueue[0];
        n_process_bg_readyQueue--;

        // 배열 원소를 한 칸씩 앞으로 복사 후, 마지막 원소 초기화
        for (int i=0; i<n_process_bg_readyQueue; i++){ 
            background_readyQueue[i] = background_readyQueue[i + 1];
        }
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        background_readyQueue[n_process_bg_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}

// about O(n)
ProcessPtr pop_nth_bg_readyQueue(int nth){
    if (n_process_bg_readyQueue > 0){
        ProcessPtr poped = background_readyQueue[nth];
        n_process_bg_readyQueue--;

        for (int i=nth; i<n_process_bg_readyQueue; i++){ 
            background_readyQueue[i] = background_readyQueue[i + 1];
        }
        
        // 임시 포인터를 만들어서 제거할 프로세스포인터가 임시포인터가 가리키는 곳을 가리키게 한 후,
        // 포인터를 제거한다.(free)
        ProcessPtr temp = (ProcessPtr)malloc(sizeof(struct ProcessType));
        background_readyQueue[n_process_bg_readyQueue] = temp;
        free(temp);
        return poped;
    }
    else {
        puts("error: ready queue is empty.");
        return NULL;
    }
}