#ifndef SO_LIST_H
#define SO_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "basic_types.h"
#include "util.h"
#include "flex_shrmem.h"

void print_table_header();

void print_process_list();

#endif