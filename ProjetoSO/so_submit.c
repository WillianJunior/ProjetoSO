#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "so_submit.h"

int parse_process_list(struct all_types **p_list, const size_t size, FILE* fp) {
    char str[PROC_EXEC_PATH_SIZE];
    char filename[PROC_EXEC_PATH_SIZE];
    char params[PROC_EXEC_PATH_SIZE];
    char max_time[PROC_EXEC_PATH_SIZE];
    char num_proc[PROC_EXEC_PATH_SIZE];
    char *aux;
    int i, j, k;
    char hours[3], minutes[3], seconds[3];
    int n_proc;
    all_types *p_aux;

    // initialize memory manager
    init(PROC_TABLE_SHM_KEY);

    (*p_list) = (all_types *) 0;
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
                    (*p_list) = (all_types *) malloc(sizeof(all_types));
                    (*p_list)->prev = (all_types *) 0;
                    p_aux = (*p_list);
                } else {
                    p_aux->next = (all_types *) malloc(sizeof(all_types));
                    p_aux->next->prev = p_aux;
                    p_aux = p_aux->next;
                }

                strcpy(p_aux->flex_types.p.exec_name, filename); // only works for the actual path NEED TO BE CORRECTED
                strcpy(p_aux->flex_types.p.exec_path, filename);
                p_aux->flex_types.p.status = PENDING;
                hours[0] = max_time[0]; 
                hours[1] = max_time[1]; 
                hours[2] = '\0';
                minutes[0] = max_time[3]; 
                minutes[1] = max_time[4]; 
                minutes[2] = '\0';
                seconds[0] = max_time[6]; 
                seconds[1] = max_time[7]; 
                seconds[2] = '\0';
                p_aux->flex_types.p.max_time = atoi(hours) * 3600 + atoi(minutes) * 60 + atoi(seconds);
                p_aux->flex_types.p.n_proc = (n_proc = atoi(num_proc)) < 0 ? 0 : n_proc; 
                strcpy(p_aux->flex_types.p.argv, params);
                p_aux->flex_types.p.n_req = get_unique_id_proc();

                p_aux->next = (all_types *) 0;
                j++;
            }
        }
    } while(!feof(fp));

    return j;
}

int print_proc_list(all_types *proc_list) {
    while(proc_list) {
        proc_pretty_printer(*proc_list);
        proc_list = proc_list->next;
    }
    return 0;
}

/**
 * Takes a list and appends it at the end of the table.
 */
int append_proc_list(all_types *proc_list) {
    all_types *aux;

    // initialize process table memory manager
    init(PROC_TABLE_SHM_KEY);

    aux = get_last_proc();
    while(proc_list) {
        if (!aux) { // Empty list
            aux = malloc_proc_shr_mem();
            // Using memcpy because we can change 'struct all_types' flexibly.
            // We can add or remove any field, as long as we keep:
            //    prev, next, prev_index and next_index. :)
            memcpy(aux, proc_list, sizeof(all_types));
            set_first_proc(aux);
        } else {
            aux->next = malloc_proc_shr_mem();
            memcpy(aux->next, proc_list, sizeof(all_types));
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

/**
 * Insert the new processes into a scheduler index list
 */
int refresh_index_list(all_types *proc_list, int index_key, int (*scheduler) (int np, int max_time)) {
    
    all_types *aux;
    all_types *index_aux;
    int priority_coef;
    // get the last process index already on shared memory
    int proc_index = 0;
    int priority_list_index;

    // initialize process table memory manager to find out the proc_index offset
    init(PROC_TABLE_SHM_KEY);
    if ((aux = get_last_proc()))
        proc_index = index_proc(aux)+1;
    printf("offset: %d\n", proc_index);

    // initialize the index list memory manager
    init(index_key);

    // run through the new process list
    while (proc_list) {
                
        // calculate the coeficient of priority
        priority_coef = (*scheduler) (proc_list->flex_types.p.n_proc, proc_list->flex_types.p.max_time);

        // insert the index in the apropriate order
        aux = get_first_proc();
        if (!aux) { // Empty list
            aux = malloc_proc_shr_mem();

            // set the index table effective values
            aux->flex_types.pl.priority_coef = priority_coef;
            aux->flex_types.pl.proc_index = proc_index;

            // set the control pointers for the first (and only) element
            set_first_proc(aux);
            set_last_proc(aux);

            // set the process_table->priority_list index pointer
            priority_list_index = index_proc(aux);

        } else if (aux->flex_types.pl.priority_coef < priority_coef) { // new first element

            index_aux = malloc_proc_shr_mem();
            
            // set the index table effective values
            index_aux->flex_types.pl.priority_coef = priority_coef;
            index_aux->flex_types.pl.proc_index = proc_index;

            // New one points the the old one
            index_aux->next_index = index_proc(aux);
            index_aux->next = aux;
            // The old one points to the new one
            aux->prev_index = index_proc(aux);
            aux->prev = index_aux;
            set_first_proc(index_aux);

            // set the process_table->priority_list index pointer
            priority_list_index = index_proc(index_aux);

        } else { // not any kind of first element
            // if there is at least one item we find the right spot
            while(next_proc(aux) && next_proc(aux)->flex_types.pl.priority_coef >= priority_coef) {
                aux = next_proc(aux);
            }

            if (!next_proc(aux)) { // new last
                // insert a new element
                index_aux = malloc_proc_shr_mem();

                // set the index table effective values
                index_aux->flex_types.pl.priority_coef = priority_coef;
                index_aux->flex_types.pl.proc_index = proc_index;

                aux->next = index_aux;
                aux->next_index = index_proc(index_aux);

                index_aux->prev = aux;
                index_aux->prev_index = index_proc(aux);

                set_last_proc(index_aux);

            } else {
                // insert a new element
                index_aux = malloc_proc_shr_mem();
                
                // set the index table effective values
                index_aux->flex_types.pl.priority_coef = priority_coef;
                index_aux->flex_types.pl.proc_index = proc_index;

                index_aux->next = next_proc(aux);
                index_aux->next_index = aux->next_index;
                index_aux->prev = aux;
                index_aux->prev_index = index_proc(aux);

                aux->next = index_aux;
                aux->next_index = index_proc(index_aux);

                index_aux->next->prev = index_aux;
                index_aux->next->prev_index = index_proc(index_aux);
            }

            // set the process_table->priority_list index pointer
            priority_list_index = index_proc(index_aux);
        }

        if (index_key == COEF_LIST_1_SHM_KEY)  // THIS    IS     SHIT!!!!!!!!!!!!!!!!!
            proc_list->flex_types.p.sjf_sch_index = priority_list_index;  // NEED TO IMPROVE FLEXIBILITY
        else                                                             // maybe we can receive the reference to where to write as a parameter
            proc_list->flex_types.p.ljf_sch_index = priority_list_index;
        proc_list = proc_list->next;
        proc_index++;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    FILE *fp;
    all_types *p_list;

    const char* filename;

    int idsem_sch_submit;
    int idsem_proc_table_mutex;

    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <all_types file>.\n");
        exit(1);
    }

    // get a communication channel between the scheduler and so_submit using a semaphore
    if ((idsem_sch_submit = semget(SCH_SBMT_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
        printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
        exit(1);
    }

    // start the mutex to provide only one read/write access
    if ((idsem_proc_table_mutex = semget(PROC_TABLE_MUTEX_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
        printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
        exit(1);
    }

    filename = argv[1];

    fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Can't open specified file: <%s>.\n", filename);
        exit(1);
    }

    parse_process_list(&p_list, SHM_BASE_PROC_NUMBER, fp);
    fclose(fp);

    // get the process table access
    sem_op(idsem_proc_table_mutex, 0);
    sem_op(idsem_proc_table_mutex, 1);

    // add the processes to the index lists
    refresh_index_list(p_list, COEF_LIST_1_SHM_KEY, sjf_schd);
    refresh_index_list(p_list, COEF_LIST_2_SHM_KEY, large_slow_proc_schd);

    // push the all_types read into shared memory's all_types vector
    append_proc_list(p_list);

    // release the process table
    sem_op(idsem_proc_table_mutex, -1);

    // send a signal to the scheduler
    sem_op_nblock(idsem_sch_submit, -1);

    return 0;
}
