#ifndef SO_SCHEDULE_H
#define SO_SCHEDULE_H

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <errno.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "spawner.h"
#include "util.h"
#include "basic_types.h"

#define SORTING_TIMEOUT 60

void sorter();

void freed_proc_daemon ();
void new_submit_daemon ();

#endif