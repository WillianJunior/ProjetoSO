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

#include "flex_shrmem.h"

/**
 * Private things.
 */
#define BITMAP_BLOCK_SIZE    sizeof(unsigned char) * 8
#define BITMAP_SIZE          SHM_BASE_PROC_NUMBER / BITMAP_BLOCK_SIZE

/**
 * Not exported functions.
 */
unsigned char* get_bitmap_shr_mem(int key);
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

/**
 * Returns pointer to the beginning of the all_types' segment.
 */
all_types* get_proc_shr_mem(int key) {
    int idshm, shm_status = 1;
    static int *p_header = (int *) 0;

    if(p_header)
        return (all_types *) (p_header + 2);

    // Instantiates a new shared memory segment or gets the id of the segment already instanciated
    if ((idshm = shmget(key, 2 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(all_types), IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST) {
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            exit(1); // TODO: Handle it properly.
        }

        // If the shm already exists we get the id only
        shm_status = 0;
        if ((idshm = shmget(key, 2 * sizeof(int) + SHM_BASE_PROC_NUMBER * sizeof(all_types), 0x1ff)) < 0) { 
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            exit(1); // TODO: Handle it properly.
        }
    }

    // Attach to the shared memory
    p_header = (int *) shmat(idshm, (char *) 0, 0);
    if (p_header == (int *) -1) { 
        fprintf(stderr, "Error attaching to shared mem: \n%s\n", strerror(errno));
        exit(1);
    }
    
    if (shm_status) {
        (*p_header)       = -1; // Pointer to the beginning of the list.
        (*(p_header+1))   = -1; // Pointer to the end of the list.
    }

    // Sets "friendly" (helper) global variable.
    base_proc = (all_types *) (p_header + 2);
    shm = p_header;

    return (all_types *) (p_header + 2);
}

/**
 * Allocates space for a all_types unity in the table.
 */
all_types* malloc_proc_shr_mem(int key) {
    int i, j, found;
    unsigned char block;
    unsigned char mask = 1;

    get_bitmap_shr_mem(key);

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

    if(!found) return (union all_types*) 0; // Return NULL

    shm_map_set(i, j);
    return map2proc(i, j);
}

/**
 * Returns the index of a given all_types unit.
 */
int index_proc(all_types* proc) {
    if(!proc)
        return -1;
    return abs(proc - base_proc);
}

/**
 * Maps a bit index and position to a all_types unit.
 */
all_types* map2proc(int index, int position) { // TODO: Convert to macro.
   int offset;
   offset = index * BITMAP_BLOCK_SIZE + position;
   return base_proc + offset;
}

/**
 * Returns pointer to the last element of the list.
 */
all_types* get_first_proc(int key) {
    int *p_base;
    all_types* p_shm;

    p_shm = get_proc_shr_mem(key);
    p_base = (int *) p_shm;
    p_base -= 2;

    if(*p_base >= 0)
        return p_shm + (*p_base);
    else
        return (all_types *) 0;
}

/**
 * Sets the given all_types unit as the first one.
 */
int set_first_proc(all_types* proc) {
    if(!proc)
        return -1;

    proc->prev = (all_types *) 0;
    proc->prev_index = -1;
    (*shm) = index_proc(proc);
    return 0;
}

/**
 * Returns pointer to the first element of the list. Useful when pushing new records.
 */
all_types* get_last_proc(int key) {
    int *p_base;
    all_types* p_shm;

    p_shm = get_proc_shr_mem(key);
    p_base = (int *) p_shm;
    p_base--;

    if((*p_base) >= 0)
        return p_shm + (*p_base);
    else
        return (all_types *) 0;
}

/**
 * Sets the given all_types unit as the last one.
 */
int set_last_proc(all_types* proc) {
    if(!proc)
        return -1;

    proc->next = (all_types *) 0;
    proc->next_index = -1;
    (*(shm+1)) = index_proc(proc);
    return 0;
}

/**
 * Returns the previous all_types units in the list.
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
 * Returns the next all_types unit in the list.
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
 * Returns pointer to the beginning of the bitmap segment.
 */
unsigned char* get_bitmap_shr_mem(int key) {
    int idshm;
    static unsigned char *block = (unsigned char*) 0;

    // Performance optimization, although, in this module, we use the global variable most of the times.
    if(block)
        return block;

    // Instantiate a new shared mem segment or get the id of the segment already instanciated
    if ((idshm = shmget(key+1, BITMAP_SIZE, IPC_CREAT|IPC_EXCL|0x1ff)) < 0) { 
        // if the shm return error and it is not already exist
        if (errno != EEXIST) {
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            exit(1); //TODO: Handle it properly.
        }

        // if the shm already exists we get the id only
        if ((idshm = shmget(key+1, BITMAP_SIZE, 0x1ff)) < 0) { 
            fprintf(stderr, "Error creating shared mem: \n%s\n", strerror(errno));
            exit(1); //TODO: Handle it properly.
        }
    }

    // Attach to the shared memory
    block = (unsigned char *) shmat(idshm, (char *) 0, 0);
    if (block == (unsigned char *) -1) { 
        fprintf(stderr, "Error attaching to shared mem: \n%s\n", strerror(errno));
        exit(1);
    }

    // Sets "friendly" (helper) global variable.
    bitmap = block;
    return block;
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
