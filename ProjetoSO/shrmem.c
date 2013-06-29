/**
 * Universidade de Brasília - UnB
 * Alunos: Alexandre Lucchesi Alencar - 09/0104471
 *         Willian Júnior -
 *
 * Sistemas Operacionais - Profa. Alba Melo
 *
 * Trabalho 1 - Escalonador de Processos
 *
 */

#include "shrmem.h"

/**
 * Private things.
 */
#define BITMAP_BLOCK_SIZE    sizeof(unsigned char) * 8
#define BITMAP_SIZE          SHM_BASE_PROC_NUMBER / BITMAP_BLOCK_SIZE
#define BITMAP_KEY 0x1928

/**
 * Not exported functions.
 */
int get_proc_shr_mem();
int get_bitmap_shr_mem();
process* map2proc(int index, int position);
void shm_map_set(int index, int position);
void shm_map_reset(int index, int position);

/**
 * Helper global variables used locally for performance purposes.
 *
 * NOTE: Their corresponding functions (get_proc_shr_mem() and get_bitmap_shr_mem()) are also 
 *       optimized but, even so, they represent much more overhead.
 */
static unsigned char *bitmap = (unsigned char*) 0;
static int *shm = (int *) 0;
static process *base_proc = (process *) 0;

/**
 * Initializes memory manager.
 *
 * IMPORTANT: Modules linking to this module must call this function once, before calling any other function. 
 *            Otherwise, it's not going to work properly.
 */
int init() {
    if(get_bitmap_shr_mem() < 0 || get_proc_shr_mem() < 0) {
        fprintf(stderr, "An error occured while initializing memory manager: \n%s\n", strerror(errno));
        exit(1);
    }
    return 0;
}

/**
 * Returns pointer to the beginning of the process' segment.
 */
int get_proc_shr_mem() {
    int idshm, shm_status = 1;

    // Instantiates a new shared memory segment or gets the id of the segment already instanciated
    if ((idshm = shmget(SHM_KEY, 3 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(process), IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST)
            return -1;

        // If the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(SHM_KEY, 3 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(process), 0x1ff)) < 0)
            return -1;
    }

    // Attach to the shared memory
    shm = (int *) shmat(idshm, (char *) 0, 0);
    if (shm == (int *) -1)
        return -1;

    // Initializes empty table.
    if (shm_status) {
        (*shm)       = -1; // Pointer to the beginning of the list.
        (*(shm+1))   = -1; // Pointer to the end of the list.
        (*(shm+2))   = 0;  // N_Req (auto-increment)
    }

    // Sets another "friendly" (helper) global variable.
    base_proc = (process *) (shm + 3);

    return 0;
}

/**
 * Allocates space for a process in the table.
 */
process* malloc_proc_shr_mem() {
    int i, j, found;
    unsigned char block;
    unsigned char mask = 1;

    found = 0;
    for(i=0; i<BITMAP_SIZE; i++) {
        block = *(bitmap+i);
        for(j=0; j<BITMAP_BLOCK_SIZE; j++) {
            if((mask & block) == 0) {
                found = 1;
                break;
            } else {
                block >>= 1;
            }
        }
        if(found) break;
    }

    if(!found) return (struct process*) 0; // Return NULL
    //DEBUG
    // printf("index = %d\n", i);
    // printf("position = %d\n\n", j);
    shm_map_set(i, j);
    return map2proc(i, j);
}

/**
 * Removes the given process from table, updates pointers and frees the space in the bitmap.
 */
int free_proc_shr_mem(process* proc) {
    int index;
    div_t res;
    process *prev, *next;

    if(!proc)
        return -1;

    prev = prev_proc(proc); // Get previous element.
    next = next_proc(proc); // Get next element.

    if(prev && next) { // proc is between two elements.
        // Just update pointers.
        prev->next = next;
        prev->next_index = proc->next_index;
        next->prev = prev;
        next->prev_index = proc->prev_index;
    } else if(prev) { // proc is the last element.
        prev->next = (process *) 0;
        prev->next_index = -1;
        set_last_proc(prev);
    } else if(next) { // proc is the first element.
        next->prev = (process *) 0;
        next->prev_index = -1;
        set_first_proc(next);
    } else {         // proc is a singleton element. 
        set_first_proc((process *) 0);
        set_last_proc((process *) 0);
    }

    index = index_proc(proc);
    res = div(index, BITMAP_BLOCK_SIZE);

    shm_map_reset(res.quot, res.rem);

    return 0;
}

/**
 * Clean the process' table completely.
 *
 * TODO: Optimize this function just setting -1 in the header and "0ing" the bitmap.
 *       It just traverses the list freeing one by one
 */
int free_all_proc_shr_mem() {
    process *p, *aux;

    p = get_first_proc();
    while(p) {
        aux = next_proc(p);
        free_proc_shr_mem(p);
        p = aux;
    }
    return 0;
}

/**
 * Returns the index of a given process.
 */
int index_proc(process* proc) {
    if(!proc)
        return -1;
    return abs(proc - base_proc);
}

/**
 * Maps a bit index and position to a process.
 */
process* map2proc(int index, int position) { // TODO: Convert to macro.
   int offset;
   offset = index * BITMAP_BLOCK_SIZE + position;
   return base_proc + offset;
}

/**
 * Returns pointer to the last element of the list.
 */
process* get_first_proc() {
    if((*shm) >= 0)
        return base_proc + (*shm);
    else
        return (process *) 0;
}

/**
 * Sets the given process as the first one.
 * 
 * If NULL is passed, it removes the reference for the first element of the list.
 */
int set_first_proc(process* proc) {
    if(!proc) {
        (*shm) = -1;
    } else {
        proc->prev = (process *) 0;
        proc->prev_index = -1;
        (*shm) = index_proc(proc);
    }
    return 0;
}

/**
 * Returns pointer to the first element of the list. Useful when pushing new records.
 */
process* get_last_proc() {
    if((*(shm+1)) >= 0)
        return base_proc + (*(shm+1));
    else
        return (process *) 0;
}

/**
 * Sets the given process as the first one.
 *
 * If NULL is passed, it removes the reference for the first element of the list.
 */
int set_last_proc(process* proc) {
    if(!proc) {
        (*(shm+1)) = -1;
    } else {
        proc->next = (process *) 0;
        proc->next_index = -1;
        (*(shm+1)) = index_proc(proc);
    }
    return 0;
}

/**
 * Returns the previous process in the list.
 */
process* prev_proc(process* proc) {
    if(!proc)
        return (process *) 0;

    if(proc->prev_index >= 0)
        return base_proc + proc->prev_index;
    else
        return (process *) 0;
}

/**
 * Returns the next process in the list.
 */
process* next_proc(process* proc) {
    if(!proc)
        return (process *) 0;
    
    if(proc->next_index >= 0)
        return base_proc + proc->next_index;
    else
        return (process *) 0;
}

/**
 * It returns the next id available following an auto-increment strategy.
 */
int get_unique_id_proc() {
    return ++(*(shm+2));
}

/**
 * Returns pointer to the beginning of the bitmap segment.
 */
int get_bitmap_shr_mem() {
    int idshm, shm_status = 1;

    // Instantiate a new shared mem segment or get the id of the segment already instantiated.
    if ((idshm = shmget(BITMAP_KEY, BITMAP_SIZE, IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST)
            return -1;
        // if the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(BITMAP_KEY, BITMAP_SIZE, 0x1ff)) < 0)
            return -1;
    }

    // Attach to the shared memory
    bitmap = (unsigned char *) shmat(idshm, (char *) 0, 0);
    if (bitmap == (unsigned char *) -1)
        return -1;

    return 0;
}

/**
 * Sets 1 to the bit specified by index and position.
 */
void shm_map_set(int index, int position) { // TODO: Convert to macro.
    unsigned char mask;
    mask = 1 << position;
    *(bitmap+index) |= mask;
}

/**
 * Sets 0 to the bit specified by index and position.
 */
void shm_map_reset(int index, int position) { // TODO: Convert to macro.
    unsigned char mask;
    mask = ~(1 << position);
    *(bitmap+index) &= mask;
}
