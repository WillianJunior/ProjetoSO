#include <stdio.h>
#include <stdlib.h>
#include "so_submit.h"

#define MAX_LINE_LENGTH 100

typedef struct process process;

int parse_process_list(struct process** p_list, const FILE* fp) {
    char str[MAX_LINE_LENGTH];

    fgets(&str, MAX_LINE_LENGTH, fp);



}

int main(int argc, char *argv[]) {

    // Alexandre's variables
    FILE *fp;
    process *p_list;
    const char* filename;
    int status;

    // Willian's variables
    process x[10];
    
    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }

    filename = argv[1];

    fp = fopen(filename);
    if(!fp) {
        fprintf(stderr, "Can't open specified file: <%s>.\n", filename);
        exit(1);
    }

    status = parse_process_list(fp, &p_list);

    if ((idshm = shmget(0x1223, sizeof(int),IPC_CREAT|0x1ff)) < 0) { 
    	printf("erro na criacao da fila\n"); exit(1);
    }

	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) { 
		printf("erro no attach\n"); exit(1);
	}

    return 0;
}
