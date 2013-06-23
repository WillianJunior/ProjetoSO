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

	/*************************************************/
    /** shared mem structure:                       **/
    /*************************************************/
    /** actual size of process table                **/
    /** process table space in use                  **/
    /** mem alocation map                           **/
    /** process table vector                        **/
    /*************************************************/

/**********************************************************/
/*** 				get_proc_shr_mem 					***/
/**********************************************************/
/*** If not instanciated, instanciate and attatch the	***/
/*** process shared memory. If already instanciated, 	***/
/*** returns the pointer for the shared memory.			***/
/**********************************************************/
/*** Returns by value: pshm								***/
/*** Returns NULL on failure							***/
/**********************************************************/
int *get_proc_shr_mem ();

/**********************************************************/
/*** 				add_proc_shr_mem 					***/
/**********************************************************/
/*** Insert a process in the process table on the		***/
/*** shared memory referenced by the pshm pointer		***/
/**********************************************************/
/*** Returns 0 on success								***/
/*** Returns -1 on failure								***/
/**********************************************************/
int add_proc_shr_mem (int *pshm, process *proc);

#endif
