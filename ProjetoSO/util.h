/**
 * Universidade de Brasília - UnB
 * Alunos: Alexandre Lucchesi Alencar - 09/0104471
 *         Willian Júnior - 09/0135806
 *
 * Sistemas Operacionais - Profa. Alba Melo
 *
 * Trabalho 1
 *
 * This code can be download in: https://github.com/WillianJunior/ProjetoSO.git
 *
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "basic_types.h"

char* sec2str(unsigned long seconds);

//void proc_pretty_printer(struct process proc);
void proc_pretty_printer(struct all_types proc);

// blocking semop
int sem_op (int idsem, int n);

// blocking semop sensitive to signals
int sem_op_sens (int idsem, int n);

// non-blocking semop
int sem_op_nblock (int idsem, int n);

// reset the semaphore to zero
void sem_reset (int idsem);

int sem_kill (int idsem);

int shm_kill (int idshm);

int msg_kill (int idmsg);

#endif
