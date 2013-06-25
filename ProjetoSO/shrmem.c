#include "shrmem.h"

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


int *get_proc_shr_mem_access () {
    
    int *pshm; 
    int *pshm_amap; 
    int idshm;
    int shm_status = 1;
    int i;

    // instanciate a new shared mem segment or get the id of the segment already instanciated
    if ((idshm = shmget(SHM_KEY, 3*sizeof(int) + SHM_BASE_PROC_NUMBER*(sizeof(process) + sizeof(int)),IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shmget return error and it is not already exist
        if (errno != EEXIST) {
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            return NULL;
        }

        // if the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(SHM_KEY, 3*sizeof(int) + SHM_BASE_PROC_NUMBER*(sizeof(process) + sizeof(int)),0x1ff)) < 0) { 
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno)); // TODO: extract all prints from the function to the main
            return NULL;
        }
    }

    // attatch the shared mem
    pshm = (int *) shmat(idshm, (char *)0, 0);
    if (pshm == (int *)-1) { 
        fprintf(stderr, "Error attaching shared mem: \n%s\n", strerror(errno));
    }

    // table all size and used size variables initialized if it hasn't been already
    if (shm_status) {
        *(pshm) = SHM_BASE_PROC_NUMBER;
        *(pshm+sizeof(int)) = SHM_BASE_PROC_NUMBER;
        *(pshm+2*sizeof(int)) = pshm+(SHM_BASE_PROC_NUMBER+3)*sizeof(int);

        pshm_amap = pshm + 3*sizeof(int);

        // initialize the allocation vector with all processes slots free
        for (i=0; i<SHM_BASE_PROC_NUMBER; i++)
            *(pshm_amap+i) = 0;
    }

    return pshm;

}

int add_proc_shr_mem (int *pshm, process *proc) {
    
    int size;
    int free_spots;
    int *alloc_vector;
    int *proc_vector;
    int *first_process;
    int prev_proc;
    int i;

    // find a free spot, if exists
    free_spots = *(pshm+sizeof(int));
    if(free_spots == 0)
        return -1;
    
    size = *pshm;
    alloc_vector = pshm+3*sizeof(int);

    for (i=0; i<size; i++) 
        if(alloc_vector[i]==0)
            break;

    // allocate a spot for the process
    alloc_vector[i] = 1;
    *pshm = *pshm-1;

    // put the process in the process table
    proc_vector = alloc_vector + SHM_BASE_PROC_NUMBER * sizeof(int);
    memcpy(proc_vector+i*sizeof(process), proc, sizeof(process));

    // rearrange the linked list pointers
    prev_proc = proc_vector + (SHM_BASE_PROC_NUMBER - free_spots)*sizeof(process);
    if (prev_proc == proc_vector)
        proc->next = NULL;
    else {
        // from here!!!
        
    }
    *(pshm+sizeof(int)) = *(pshm+sizeof(int)) - 1;

    // returns the number of free spots available
    return free_spots;
}

int main(int argc, char *argv[]) {

    FILE *fp;
    process p_list[SHM_BASE_PROC_NUMBER];
    int p_count, *pp_list, *shm_pcounter;
    const char* filename;

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

    
    pp_list = get_proc_shr_mem_access();

    proc_pretty_printer(p_list[0]);


    // push the processes read into shared memory's process vector
    for (i=0; i<p_count; i++) {
        if (add_proc_shr_mem(pp_list, &p_list[i]) < 0) {
            printf("erro\n");
            exit(1);
        }
    }

    memcpy(&p_list[8], pp_list+(2+SHM_BASE_PROC_NUMBER)*sizeof(int), sizeof(process));
    proc_pretty_printer(p_list[8]);

    return 0;
}
