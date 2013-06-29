#ifndef FLEX_SHR_MEM_H
#define FLEX_SHR_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

#include "basic_types.h"

// COMMENTS ABOUT THE FUNCTIONS ARE TEMPORARILY IN 'shrmem.c'.
all_types* get_proc_shr_mem(int key);

all_types* malloc_proc_shr_mem(int key);

all_types* get_first_proc(int key);

int set_first_proc(all_types* proc);

all_types* get_last_proc(int key);

int set_last_proc(all_types* proc);

int index_proc(all_types* proc);

all_types* next_proc(all_types* proc);

all_types* last_proc(all_types* proc);

#endif
