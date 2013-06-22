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

/**********************************************************/
/*** 				get_proc_shr_mem 					***/
/**********************************************************/
/*** If not instanciated, instanciate and attatch the	***/
/*** process shared memory. If already instanciated, 	***/
/*** returns the pointer for the shared memory.			***/
/**********************************************************/
/*** Returns by value: pshm								***/
/*** Returns by reference:  ***/
/***  ***/
/**********************************************************/
// instanciate and attach a process shared memory. returns the shm id
int *get_proc_shr_mem ();

int add_proc_shr_mem(process *proc, int *pshm);

#endif
