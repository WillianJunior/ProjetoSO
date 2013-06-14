#ifndef SO_SUBMIT_H
#define SO_SUBMIT_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include "basic_types.h"

int parse_process_list(struct process** p_list, const FILE* fp);


#endif
