#ifndef SHR_MEM_H
#define SHR_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

#include "basic_types.h"

// COMMENTS ABOUT THE FUNCTIONS ARE TEMPORARILY IN 'shrmem.c'.
int init(int key);

all_types* malloc_proc_shr_mem();

int free_proc_shr_mem(all_types* proc);

int free_all_proc_shr_mem();

all_types* get_first_proc();

int set_first_proc(all_types* proc);

all_types* get_last_proc();

int set_last_proc(all_types* proc);

all_types* prev_proc(all_types *);

all_types* next_proc(all_types *);

int index_proc(all_types* proc);

all_types* next_proc(all_types* proc);

all_types* last_proc(all_types* proc);

int get_unique_id_proc();

all_types* get_proc_by_index(int index);

#endif
