#include <stdio.h>
#include <stdlib.h>
#include "so_submit.h"

#define MAX_LINE_LENGTH 100

typedef struct process process;

int parse_process_list(struct process p_list[], const size_t size, FILE* fp) {
    char str[MAX_LINE_LENGTH];
    char filename[MAX_LINE_LENGTH];
    char params[MAX_LINE_LENGTH];
    char max_time[MAX_LINE_LENGTH];
    char num_proc[MAX_LINE_LENGTH];
    char *aux;
    int i, j;

    i = 0;
    j = 0;
    do { 
        if(fgets(str, MAX_LINE_LENGTH, fp)) {
            if(str[0] == '%') continue;
            aux = strtok(str, "= ");
            if(aux[0] == '\n') continue;
            aux = strtok(NULL, "= ");
            aux[strlen(aux)-1] = '\0';
            switch(i) {
                case 0:
                    printf("filename: %s\n", aux);
                    i++;
                    break;
                case 1:
                    printf("params:%s\n", aux);
                    i++;
                    break;
                case 2:
                    printf("max_time: %s\n", aux);
                    i++;
                    break;
                default:
                    printf("n_proc: %s\n", aux);
                    i=0;
            }
            if((i == 0) && (j < size - 1)) {
                //(p_list + j)->exec_path = 
                j++;
            }
        } else {
            
        }
    } while(!feof(fp));

    return 0;
}

int main(int argc, char *argv[]) {

    // Alexandre's variables
    FILE *fp;
    process *p_list;
    const char* filename;
    int status;

    // Willian's variables
    process x[10];
    int idshm;
    int *pshm;
    
    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }

    filename = argv[1];

    fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Can't open specified file: <%s>.\n", filename);
        exit(1);
    }

    status = parse_process_list(&p_list, fp);

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
