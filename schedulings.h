#ifndef SCHEDULINGS_H_
#define SCHEDULINGS_H_

#include "processes.h"

bool job_schedule();
bool job_schedule_MLQ(int queue_num);
void execute_FCFS(int n_process);
void execute_SJF(int n_process, bool is_preemptive);
void execute_Priority(int n_process, bool is_preemptive);
void execute_RR(int n_process, int time_quantum);
void execute_Loterry(int n_process, int time_quantum);
void execute_HRN(int n_process);
void execute_MLQ(int n_process, int time_quantu, int priority_criteria);
void execute_MLFQ(int n_process, int time_quantum, int priority_criteria, int aging_condition, int demote_condition);
void execute_LIF(int n_process, bool is_preemptive);

extern int t; // scheduling time
extern int scheduling_start_time;
extern int idle_time;
extern ProcessPtr crp; // crp : Current Running Process

#endif