#ifndef SPAWNER_H
#define SPAWNER_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "util.h"
#include "basic_types.h"

#define MSGQ_KEY 0x1927
#define SEM_KEY 0x1927
#define ZOMBIE_KILLER_TIMEOUT 20

void zombie_killer();

void proc_killer ();

#endif