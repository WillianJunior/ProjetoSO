#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "basic_types.h"

char* sec2str(unsigned long seconds);

//void proc_pretty_printer(struct process proc);
void proc_pretty_printer(struct all_types proc);
void proc_index_test_pretty_printer(struct all_types proc);

// blocking semop
int sem_op (int idsem, int n);

// non-blocking semop
int sem_op_nblock (int idsem, int n);

// reset the semaphore to zero
void sem_reset (int idsem);

#endif
