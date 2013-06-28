#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "basic_types.h"

void proc_pretty_printer(struct process proc);

void sem_op (int idsem_free_proc, int n);

#endif