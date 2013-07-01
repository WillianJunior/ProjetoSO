#ifndef SPAWNER_H
#define SPAWNER_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "shrmem.h"
#include "util.h"
#include "basic_types.h"

#define ZOMBIE_KILLER_TIMEOUT 20

void zombie_killer();

void proc_killer ();

void finalize ();

#endif