#ifndef PROCESSES_H_
#define PROCESSES_H_

#include "config.h"

// test set
void create_test_processes();
// random test set
void create_processes(int n_proces, int n_io);

void print_all_processes(ProcessPtr *p, int n_process);
void print_process(ProcessPtr p);

#endif