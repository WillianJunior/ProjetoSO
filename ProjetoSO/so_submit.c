#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "basic_types.h"
#include "so_submit.h"

#define MAX_LINE_LENGTH 100

typedef struct process process;

int parse_process_list(struct process** p_list, const FILE* fp) {
    char str[MAX_LINE_LENGTH];

    fgets(&str, MAX_LINE_LENGTH, fp);



}

int main(int argc, char *argv[]) {

    int *pshm, idshm;
    int i, j;
    FILE *fp;
    process *p_list;
    const char* filename;
    int status;

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
