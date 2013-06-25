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

	/**************************************************/
    /** shared mem structure:                        **/
    /**************************************************/
    /** actual size of process table                 **/
    /** process table free space					 **/
	/** process table first process					 **/
    /** mem alocation map                            **/
    /** process table vector                         **/
    /**************************************************/
	/** The processes on memory are organized as a 	 **/
	/** static size vector and ordered as a linked 	 **/
	/** list. There is a variable that indicates     **/
	/** where the linked list begins  There is an 	 **/
	/** allocation map especifying the free spots on **/
	/** the vector. 								 **/
	/**************************************************/

/**********************************************************/
/*** 				get_proc_shr_mem_access				***/
/**********************************************************/
/*** If not instanciated, instanciate and attatch the	***/
/*** process shared memory. If already instanciated, 	***/
/*** returns the pointer for the shared memory.			***/
/**********************************************************/
/*** Returns by value: pshm								***/
/*** Returns NULL on failure							***/
/**********************************************************/
int *get_proc_shr_mem_access ();

/**********************************************************/
/*** 				add_proc_shr_mem 					***/
/**********************************************************/
/*** Insert a process in the process table on the		***/
/*** shared memory referenced by the pshm pointer		***/
/**********************************************************/
/*** Returns the number of free slots on success		***/
/*** Returns -1 on failure								***/
/**********************************************************/
int add_proc_shr_mem (int *pshm, process *proc);

/**********************************************************/
/***				remove_proc_shr_mem					***/
/**********************************************************/
/*** Remove the process referenced by a given index.	***/
/*** Obs: The process list order is the linked list 	***/
/*** order.											 	***/
/**********************************************************/
/*** Returns the number of free slots on success		***/
/*** Returns -1 on failure								***/
/**********************************************************/

int remove_proc_shr_mem (int *pshm, int index);

/**********************************************************/
/*** 				get_proc_shr_mem 					***/
/**********************************************************/
/*** Get the process referenced by a given index. 		***/
/**********************************************************/
/*** Returns 0 on success								***/
/*** Returns -1 on failure								***/
/**********************************************************/
int get_proc_shr_mem (int *pshm, int index, process *proc);

#endif
