#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "so_submit.h"

int parse_process_list(struct process **p_list, const size_t size, FILE* fp) {
    char str[PROC_EXEC_PATH_SIZE];
    char filename[PROC_EXEC_PATH_SIZE];
    char params[PROC_EXEC_PATH_SIZE];
    char max_time[PROC_EXEC_PATH_SIZE];
    char num_proc[PROC_EXEC_PATH_SIZE];
    char *aux;
    int i, j, k;
    char hours[3], minutes[3], seconds[3];
    int n_proc;
    process *p_aux;

    (*p_list) = (process *) 0;
    i = 0;
    j = 0;
    do { 
        if(fgets(str, PROC_EXEC_PATH_SIZE, fp)) {
            if(str[0] == '%') continue;
            aux = strtok(str, "= ");
            if(aux[0] == '\n') continue;
            aux = strtok(NULL, "=");
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
            if(i == 0) {
                if(!(*p_list)) {
                    (*p_list) = (process *) malloc(sizeof(process));
                    (*p_list)->prev = (process *) 0;
                    p_aux = (*p_list);
                } else {
                    p_aux->next = (process *) malloc(sizeof(process));
                    p_aux->next->prev = p_aux;
                    p_aux = p_aux->next;
                }

                strcpy(p_aux->exec_name, filename); // only works for the actual path NEED TO BE CORRECTED
                strcpy(p_aux->exec_path, filename);
                p_aux->status = PENDING;
                hours[0] = max_time[0]; 
                hours[1] = max_time[1]; 
                hours[2] = '\0';
                minutes[0] = max_time[3]; 
                minutes[1] = max_time[4]; 
                minutes[2] = '\0';
                seconds[0] = max_time[6]; 
                seconds[1] = max_time[7]; 
                seconds[2] = '\0';
                p_aux->max_time = atoi(hours) * 3600 + atoi(minutes) * 60 + atoi(seconds);
                p_aux->n_proc = (n_proc = atoi(num_proc)) < 0 ? 0 : n_proc; 
                strcpy(p_aux->argv, params);

                p_aux->next = (process *) 0;
                j++;
            }
        }
    } while(!feof(fp));

    return j;
}

int print_proc_list(process *proc_list) {
    while(proc_list) {
        proc_pretty_printer(*proc_list);
        proc_list = proc_list->next;
    }
    return 0;
}

/**
 * Takes a list and appends it at the end of the table.
 */
int append_proc_list(process *proc_list) {
    process *aux;

    aux = get_last_proc();
    while(proc_list) {
        if (!aux) { // Empty list
            aux = malloc_proc_shr_mem();
            // Using memcpy because we can change 'struct process' flexibly.
            // We can add or remove any field, as long as we keep:
            //    prev, next, prev_index and next_index. :)
            memcpy(aux, proc_list, sizeof(process));
            set_first_proc(aux);
        } else {
            aux->next = malloc_proc_shr_mem();
            memcpy(aux->next, proc_list, sizeof(process));
            aux->next_index = index_proc(aux->next);
            aux->next->prev = aux;
            aux->next->prev_index = index_proc(aux);
            aux = aux->next;
        }

        set_last_proc(aux);
        proc_list = proc_list->next;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    FILE *fp;
    process *p_list;

    int p_count;
    const char* filename;

    int idsem_sch_submit;

    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }

    // get a communication channel between the scheduler and so_submit using a semaphore
    if ((idsem_sch_submit = semget(SCH_SBMT_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
        printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
        exit(1);
    }

    filename = argv[1];

    fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Can't open specified file: <%s>.\n", filename);
        exit(1);
    }

    p_count = parse_process_list(&p_list, SHM_BASE_PROC_NUMBER, fp);
    fclose(fp);

    // push the processes read into shared memory's process vector
    init();
    append_proc_list(p_list);

    // send a signal to the scheduler
    sem_op_nblock(idsem_sch_submit, -1);

    return 0;
}
