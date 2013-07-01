#ifndef SO_LIST_H
#define SO_LIST_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "basic_types.h"
#include "util.h"
#include "shrmem.h"

void print_table_header();

void print_process_list();

#endif