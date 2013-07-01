#ifndef SO_SHUTDOWN_H
#define SO_SHUTDOWN_H

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

void hard_shutdown();

#endif