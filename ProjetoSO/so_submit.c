#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "basic_types.h"

typedef struct process process;

int main(int argc, char *argv[]) {

 //   FILE *fp;
    struct process x[10];

    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }

//    parse

    if ((idshm = shmget(0x1223, sizeof(int),IPC_CREAT|0x1ff)) < 0) { 
    	printf("erro na criacao da fila\n"); exit(1);
    }

	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) { 
		printf("erro no attach\n"); exit(1);
	}

    return 0;
}
