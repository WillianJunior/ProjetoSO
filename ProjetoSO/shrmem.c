/**
 * Universidade de Brasília - UnB
 * Alunos: Alexandre Lucchesi Alencar - 09/0104471
 *         Willian Júnior -
 *
 * Sistemas Operacionais - Profa. Alba Melo
 *
 * Trabalho 1 - Escalonador de all_typesos
 *
 */

#include "shrmem.h"

/**
 * Private things.
 */
#define BITMAP_BLOCK_SIZE    sizeof(unsigned char) * 8
#define BITMAP_SIZE          SHM_BASE_PROC_NUMBER / BITMAP_BLOCK_SIZE

/**
 * Not exported functions.
 */
int get_proc_shr_mem();
int get_bitmap_shr_mem();
all_types* map2proc(int index, int position);
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
static all_types *base_proc = (all_types *) 0;
static int proc_key;
static int bitmap_key;

/**
 * Initializes memory manager.
 *
 * IMPORTANT: Modules linking to this module must call this function once, before calling any other function. 
 *            Otherwise, it's not going to work properly.
 */
int init(int key) {
    proc_key = key;
    bitmap_key = key + 1;
    if(get_bitmap_shr_mem() < 0 || get_proc_shr_mem() < 0) {
        fprintf(stderr, "An error occured while initializing memory manager: \n%s\n", strerror(errno));
        exit(1);
    }
    return 0;
}

/**
 * Returns pointer to the beginning of the all_types' segment.
 */
int get_proc_shr_mem() {
    int idshm, shm_status = 1;

    // Instantiates a new shared memory segment or gets the id of the segment already instanciated
    if ((idshm = shmget(proc_key, 3 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(all_types), IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST)
            return -1;

        // If the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(proc_key, 3 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(all_types), 0x1ff)) < 0)
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
    base_proc = (all_types *) (shm + 3);

    return 0;
}

/**
 * Allocates space for a all_types in the table.
 */
all_types* malloc_proc_shr_mem() {
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

    if(!found) return (struct all_types*) 0; // Return NULL
    //DEBUG
    // printf("(*shm) = %d\n", *shm);
    // printf("*(shm+1) = %d\n", *(shm+1));
    // printf("*(shm+2) = %d\n", *(shm+2));
    // printf("index = %d\n", i);
    // printf("position = %d\n\n", j);
    shm_map_set(i, j);
    return map2proc(i, j);
}

/**
 * Removes the given all_types from table, updates pointers and frees the space in the bitmap.
 */
int free_proc_shr_mem(all_types* proc) {
    int index;
    div_t res;
    all_types *prev, *next;

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
        prev->next = (all_types *) 0;
        prev->next_index = -1;
        set_last_proc(prev);
    } else if(next) { // proc is the first element.
        next->prev = (all_types *) 0;
        next->prev_index = -1;
        set_first_proc(next);
    } else {         // proc is a singleton element. 
        set_first_proc((all_types *) 0);
        set_last_proc((all_types *) 0);
    }

    index = index_proc(proc);
    res = div(index, BITMAP_BLOCK_SIZE);

    shm_map_reset(res.quot, res.rem);

    return 0;
}

/**
 * Clean the all_types' table completely.
 *
 * TODO: Optimize this function just setting -1 in the header and "0ing" the bitmap.
 *       It just traverses the list freeing one by one
 */
int free_all_proc_shr_mem() {
    all_types *p, *aux;

    p = get_first_proc();
    while(p) {
        aux = next_proc(p);
        free_proc_shr_mem(p);
        p = aux;
    }
    return 0;
}

/**
 * Returns the index of a given all_types.
 */
int index_proc(all_types* proc) {
    if(!proc)
        return -1;
    return abs(proc - base_proc);
}

/**
 * Maps a bit index and position to a all_types.
 */
all_types* map2proc(int index, int position) { // TODO: Convert to macro.
   int offset;
   offset = index * BITMAP_BLOCK_SIZE + position;
   return base_proc + offset;
}

/**
 * Returns pointer to the last element of the list.
 */
all_types* get_first_proc() {
    if((*shm) >= 0)
        return base_proc + (*shm);
    else
        return (all_types *) 0;
}

/**
 * Sets the given all_types as the first one.
 * 
 * If NULL is passed, it removes the reference for the first element of the list.
 */
int set_first_proc(all_types* proc) {
    if(!proc) {
        (*shm) = -1;
    } else {
        proc->prev = (all_types *) 0;
        proc->prev_index = -1;
        (*shm) = index_proc(proc);
    }
    return 0;
}

/**
 * Returns pointer to the first element of the list. Useful when pushing new records.
 */
all_types* get_last_proc() {
    if((*(shm+1)) >= 0)
        return base_proc + (*(shm+1));
    else
        return (all_types *) 0;
}

/**
 * Sets the given all_types as the first one.
 *
 * If NULL is passed, it removes the reference for the first element of the list.
 */
int set_last_proc(all_types* proc) {
    if(!proc) {
        (*(shm+1)) = -1;
    } else {
        proc->next = (all_types *) 0;
        proc->next_index = -1;
        (*(shm+1)) = index_proc(proc);
    }
    return 0;
}

/**
 * Returns the previous all_types in the list.
 */
all_types* prev_proc(all_types* proc) {
    if(!proc)
        return (all_types *) 0;

    if(proc->prev_index >= 0)
        return base_proc + proc->prev_index;
    else
        return (all_types *) 0;
}

/**
 * Returns the next all_types in the list.
 */
all_types* next_proc(all_types* proc) {
    if(!proc)
        return (all_types *) 0;
    
    if(proc->next_index >= 0)
        return base_proc + proc->next_index;
    else
        return (all_types *) 0;
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
    int idshm;

    // Instantiate a new shared mem segment or get the id of the segment already instantiated.
    if ((idshm = shmget(bitmap_key, BITMAP_SIZE, IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST)
            return -1;
        // if the shm already exists we get the id only
        if ((idshm = shmget(bitmap_key, BITMAP_SIZE, 0x1ff)) < 0)
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

all_types* get_proc_by_index(int index) {
    return (base_proc && index >= 0) ? (base_proc + index) : (all_types *) 0;
}

void remove_proc_shr_mem(all_types* proc) {
    int index1 = proc->flex_types.p.sjf_sch_index;
    int index2 = proc->flex_types.p.ljf_sch_index;

    // finaly free the process
    //init(PROC_TABLE_SHM_KEY);
    free_proc_shr_mem(proc);

    // free all the index lists
    init(COEF_LIST_1_SHM_KEY);
    free_proc_shr_mem(get_proc_by_index(index1));

    init(COEF_LIST_2_SHM_KEY);
    free_proc_shr_mem(get_proc_by_index(index2));
    // as before, this needs to be more flexible (easier to add a new scheduler to the round table)

}
