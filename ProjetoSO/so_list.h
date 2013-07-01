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
