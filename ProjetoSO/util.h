#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "basic_types.h"

//void proc_pretty_printer(union all_types proc);
void proc_pretty_printer(struct process proc);

// blocking semop
void sem_op (int idsem_free_proc, int n);

// non-blocking semop
int sem_op_nblock (int idsem_free_proc, int n);

#endif