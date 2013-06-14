#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "basic_types.h"
#include "so_submit.h"

#define MAX_LINE_LENGTH 100

typedef struct process process;

int main(int argc, char *argv[]) {

    int *pshm, *pp_list; 
    int idshm;
    int shm_status = 1;

    process *p_list;
    int p_count;

    int i, j;

    /*************************************************/
    /** shared mem structure:						**/
    /*************************************************/
    /** actual size of process table				**/
    /** process table space in use 					**/
    /** process table vector						**/
    /*************************************************/

    // instanciate a new shared mem segment or get the id of the segment already instanciated
    if ((idshm = shmget(SHM_KEY, 2*sizeof(int) + SHM_BASE_PROC_NUMBER*sizeof(process),IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
    	// if the shm return error and it is not already exist
        if (errno != EEXIST) {
            fprintf(stderr, "Error creating shared mem: \n%s\n", (char*)strerror(errno));
    	    exit(1);
        }

        // if the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(SHM_KEY, 2*sizeof(int) + SHM_BASE_PROC_NUMBER*sizeof(process),0x1ff)) < 0) { 
            fprintf(stderr, "Error creating shared mem: \n%s\n", (char*)strerror(errno));
            exit(1);
        }
    }

    // attatch the shared mem
	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) { 
		fprintf(stderr, "Error attaching shared mem: \n%s\n", (char*)strerror(errno));
	}

    // table all size and used size variables initialized if ti hasn't been already
    if (shm_status) {
        *(pshm) = SHM_BASE_PROC_NUMBER;
        *(pshm+sizeof(int)) = 0;
    }   

    pp_list = pshm+2*sizeof(int);



    return 0;
}
