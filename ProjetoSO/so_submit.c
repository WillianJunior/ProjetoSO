#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "so_submit.h"

typedef struct process process;

int parse_process_list(struct process p_list[], const size_t size, FILE* fp) {
    char str[PROC_EXEC_PATH_SIZE];
    char filename[PROC_EXEC_PATH_SIZE];
    char params[PROC_EXEC_PATH_SIZE];
    char max_time[PROC_EXEC_PATH_SIZE];
    char num_proc[PROC_EXEC_PATH_SIZE];
    char *aux;
    int i, j, k;
    char hours[3], minutes[3], seconds[3];
    int n_proc;

    i = 0;
    j = 0;
    do { 
        if(fgets(str, PROC_EXEC_PATH_SIZE, fp)) {
            if(str[0] == '%') continue;
            aux = strtok(str, "= ");
            if(aux[0] == '\n') continue;
            aux = strtok(NULL, "= ");
            aux[strlen(aux)-1] = '\0';
            switch(i) {
                case 0:
                    printf("filename: %s\n", aux);
                    i++;
                    strcpy(filename, aux);
                    break;
                case 1:
                    printf("params:%s\n", aux);
                    i++;
                    strcpy(params, aux);
                    break;
                case 2:
                    printf("max_time: %s\n", aux);
                    if(strlen(aux) != 8) {
                        fprintf(stderr, "Parsing error.\n");
                        exit(1);
                    }
                    strcpy(max_time, aux);
                    i++;
                    break;
                default:
                    printf("n_proc: %s\n", aux);
                    for(k=0; k<strlen(aux); k++) {
                        if(!isdigit(aux[k])) {
                            fprintf(stderr, "Parsing error.\n");
                            exit(1);
                        }
                    }
                    strcpy(num_proc, aux);
                    i=0;
            }
            if((i == 0) && (j < size - 1)) {
                strcpy(p_list[j].exec_path, filename);
                hours[0] = max_time[0]; 
                hours[1] = max_time[1]; 
                hours[2] = '\0';
                minutes[0] = max_time[3]; 
                minutes[1] = max_time[4]; 
                minutes[2] = '\0';
                seconds[0] = max_time[6]; 
                seconds[1] = max_time[7]; 
                seconds[2] = '\0';
                p_list[j].max_time = atoi(hours) * 3600 + atoi(minutes) * 60 + atoi(seconds);
                p_list[j].n_proc = (n_proc = atoi(num_proc)) < 0 ? 0 : n_proc; 
                strcpy(p_list[j].argv, params);
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
    process p_list[SHM_BASE_PROC_NUMBER];
    const char* filename;
    int status;

    // Willian's variables
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

    status = parse_process_list(p_list, SHM_BASE_PROC_NUMBER, fp);

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

    fclose(fp);

    return 0;
}
