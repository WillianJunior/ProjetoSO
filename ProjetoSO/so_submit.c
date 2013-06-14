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
                    //printf("filename: %s\n", aux);
                    i++;
                    strcpy(filename, aux);
                    break;
                case 1:
                    //printf("params:%s\n", aux);
                    i++;
                    strcpy(params, aux);
                    break;
                case 2:
                    //printf("max_time: %s\n", aux);
                    if(strlen(aux) != 8) {
                        fprintf(stderr, "Parsing error.\n");
                        exit(1);
                    }
                    strcpy(max_time, aux);
                    i++;
                    break;
                default:
                    //printf("n_proc: %s\n", aux);
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

    return j;
}

int main(int argc, char *argv[]) {

    FILE *fp;
    process p_list[SHM_BASE_PROC_NUMBER];
    int p_count;
    const char* filename;

    int *pshm, *pp_list; 
    int idshm;
    int shm_status = 1;

    int i, j;
    
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

    p_count = parse_process_list(p_list, SHM_BASE_PROC_NUMBER, fp);
    fclose(fp);

    // Sample code
    proc_pretty_printer(p_list[0]);
    proc_pretty_printer(p_list[1]);
    
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
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
    	    exit(1);
        }

        // if the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(SHM_KEY, 2*sizeof(int) + SHM_BASE_PROC_NUMBER*sizeof(process),0x1ff)) < 0) { 
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            exit(1);
        }
    }

    // attatch the shared mem
	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) { 
		fprintf(stderr, "Error attaching shared mem: \n%s\n", strerror(errno));
	}

    // table all size and used size variables initialized if ti hasn't been already
    if (shm_status) {
        *(pshm) = SHM_BASE_PROC_NUMBER;
        *(pshm+sizeof(int)) = 0;
    }   

    pp_list = pshm+2*sizeof(int);

    return 0;
}
