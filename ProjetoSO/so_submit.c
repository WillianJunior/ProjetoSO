#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "basic_types.h"
#include "so_submit.h"

typedef struct process process;

int main(int argc, char *argv[]) {

 //   FILE *fp;
    struct process x[10];
    int *pshm, idshm;
    int i, j;

    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }

//    parse

    /*************************************************/
    /** shared mem structure:						**/
    /*************************************************/
    /** actual size of process vector				**/
    /** process space in use 						**/
    /** process vector								**/
    /*************************************************/

    if ((idshm = shmget(SHM_KEY, 2*sizeof(int) + SHM_BASE_PROC_NUMBER*sizeof(process),IPC_CREAT|0x1ff)) < 0) { 
    	fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
    	exit(1);
    }

	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) { 
		fprintf(stderr, "Error attaching shared mem: \n%s\n", strerror(errno));
	}



    return 0;
}
