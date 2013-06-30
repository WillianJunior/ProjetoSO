#ifndef SHR_MEM_H
#define SHR_MEM_H

#define SHM_KEY 0x1927

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

#include "basic_types.h"

// COMMENTS ABOUT THE FUNCTIONS ARE TEMPORARILY IN 'shrmem.c'.
int init();

process* malloc_proc_shr_mem();

int free_proc_shr_mem(process* proc);

int free_all_proc_shr_mem();

process* get_first_proc();

int set_first_proc(process* proc);

process* get_last_proc();

int set_last_proc(process* proc);

process* prev_proc(process *);

process* next_proc(process *);

int index_proc(process* proc);

process* next_proc(process* proc);

process* last_proc(process* proc);

int get_unique_id_proc();

#endif
