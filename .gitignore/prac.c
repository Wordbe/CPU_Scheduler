// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>


// typedef enum {false, true} bool;
// struct ProcessType{
//     int pid;
//     int cpu_burst;
//     int io_burst;
//     int arrival;    
//     int priority;
// };
// typedef struct ProcessType* ProcessPtr;


// // Heap sort (using min heap)
// // ---------------------------------------
// struct Heap{
//     int size;
//     int *array;
// };

// void swap(int *a, int *b){int t = *a; *a = *b; *b = t;}

// // reheap down
// void _heapify(int *arr, int size, int idx){
//     int smallest = idx; // root
//     int left = 2*idx + 1;
//     int right = 2*idx + 2;

//     if (left < size &&
//         arr[left] < arr[smallest])
//         smallest = left;
//     if (right < size &&
//         arr[right] < arr[smallest])
//         smallest = right;
//     if (smallest != idx){
//         swap(&arr[smallest], &arr[idx]);
        
//         _heapify(arr, size, smallest);
//     }
// }
// void heap_sort(int *arr, int size){
//     // build heap
//     int parent_of_last= (size -2)/2;
//     for (int i=parent_of_last; i>=0; i--)
//         _heapify(arr, size, i);
    
//     while(size > 1){
//         swap(&arr[0], &arr[size - 1]);
//         size--;
//         _heapify(arr, size, 0);
//     }
// }

// void printArray(int* arr, int size)
// {
//     int i;
//     for (i = 0; i < size; ++i)
//         printf("%d ", arr[i]);
// }


// struct P {
//     int x;
//     int y;
// };
// typedef struct P* Pptr;

// #define N_MAX_PROCESS 30

// struct CircularQueue{
//     int front;
//     int rear;
//     int size;
//     ProcessPtr queue[N_MAX_PROCESS];
// };
// typedef struct CircularQueue* CircularQueuePtr;

// void change(int *t){
//     *t += 3;
// }

// int main() {
//     // ProcessPtr p[2];
//     // p[0] = (ProcessPtr)malloc(sizeof(struct ProcessType));
//     // memset(p[0], 0, sizeof(struct ProcessType));
//     // // p[0] = NULL;
//     // printf("%d, %d, %d, %d, %d\n", 
//     //         p[0]->pid, p[0]->cpu_burst, p[0]->io_burst, p[0]->arrival, p[0]->priority);
//     // p[0]->pid = 55;
//     // // struct ProcessType *p2 = (struct ProcessType*)malloc(sizeof(struct ProcessTpype));
//     // p[1] = (ProcessPtr)malloc(sizeof(struct ProcessType));
//     // // memcpy(p[1], p[0], sizeof(struct ProcessType));
//     // p[1] = p[0];
//     // printf("%d, %d, %d, %d, %d\n", 
//     //         p[1]->pid, p[1]->cpu_burst, p[1]->io_burst, p[1]->arrival, p[1]->priority);

//     // puts("Geek");
//     // fputs("Geekfor", stdout);
//     // puts("Geek");
//     // getchar();

//     // int arr[] = {12, 11, 13, 13, 13, 7};
//     // int n=10;
//     // int arr[n];
//     // int size = sizeof(arr)/sizeof(arr[0]);

//     // heap_sort(arr, size);

//     // printf("\nSorted array is \n");
//     // printArray(arr, size);
//     // _random_without_overlap(arr, n);
//     // printf("%ld %ld %d", sizeof(arr), sizeof(arr[0]), size);

//     // struct P *p = (struct P*) malloc(sizeof(struct P));
//     // p->x = 1;
//     // p->y = 2;
//     // struct P *q = (struct P*) malloc(sizeof(struct P));
//     // q->x = 3;
//     // q->y = 4;
//     // struct P *r = (struct P*) malloc(sizeof(struct P));
//     // r->x = 5;
//     // r->y = 6;
//     // // memset(p, 0, sizeof(struct P));
//     // // p = NULL;
    



//     // printf("array\n");
//     // struct P *arr[3];
//     // for (int i=0; i<3; i++){
//     //     printf("%d %d\n", arr[i]->x, arr[i]->y);
//     // }

//     // // for (int i=0; i<3; i++){
//     // //     arr[i] = NULL;
//     // // }

//     // // for (int i=0; i<3; i++){
//     // //     memset(arr[i], 0, sizeof(struct P));
//     // // }

//     // // for (int i=0; i<3; i++){
//     // //     printf("%d %d\n", arr[i]->x, arr[i]->y);
//     // // }

//     // arr[0] = p;
//     // arr[1] = q;
//     // arr[2] = r;

//     // // for (int i=0; i<3; i++){
//     // //     arr[i]->x = 0;
//     // //     arr[i]->y = 0;
//     // // }

//     // puts("\n");
//     // for (int i=0; i<3; i++){
//     //     printf("%d %d\n", arr[i]->x, arr[i]->y);
//     // }

//     // // for (int i=0; i<3; i++){
//     // //     memset(arr[i], 0, sizeof(struct P));
//     // // }

//     // // for (int i=0; i<3; i++){
//     // //     printf("%d %d\n", arr[i]->x, arr[i]->y);
//     // // }
   
//     // // free(p);
//     // // free(q);
//     // // free(r);

//     // for (int i=0; i<2; i++){
//     //     arr[i] = arr[i+1];
//     // }
    
//     // struct P *tmp = (struct P*)malloc(sizeof(struct P));
//     // arr[2] = tmp;
//     // free(tmp);

//     // // struct P *tmp2 = (struct P*)malloc(sizeof(struct P));
//     // // arr[1] = tmp2;
//     // // free(tmp2);
//     // // memset(arr[2], 0, sizeof(struct P));
//     // puts("\n\n");
//     // for (int i=0; i<3; i++){
//     //     printf("%d %d\n", arr[i]->x, arr[i]->y);
//     // }
//     // Pptr tmp[2];
//     // for (int i=0; i<2; i++){
//     //     Pptr a = (Pptr)malloc(sizeof(struct P));
//     //     a->x = i;
//     //     a->y = i+1;

//     //     tmp[i] = a;
//     // }
//     // for (int i=0; i<2; i++){
//     //     printf("%d %d\n", tmp[i]->x, tmp[i]->y);
//     // }

//     // for (int i=0; i<2; i++){
//     //     free(tmp[i]);
//     // }
    
//     // for (int i=0; i<2; i++){
//     //     printf("%d %d\n", tmp[i]->x, tmp[i]->y);
//     // }
    
//     // CircularQueuePtr q = (CircularQueuePtr)malloc(sizeof(struct CircularQueue));
//     // ProcessPtr p = (ProcessPtr)malloc(sizeof(struct ProcessType));
//     // p->cpu_burst = 1;

//     // q->front = 0;
//     // q->rear = 0;
//     // q->size = 0;
//     // q->queue[0] = p;

//     // printf("%d, %d\n", q->front, q->queue[0]->cpu_burst);

//     // char *a;
//     // a = "hellow";
//     // printf("%s", a);
    
//     // int t = 0;
//     // change(&t);
//     // printf("%d\n", t);

//     // char *a = "hi";
//     // printf("%d\n", strcmp(a, "./hi"));
    
//     return 0;
// }