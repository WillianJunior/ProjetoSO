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

#ifndef SO_SUBMIT_H
#define SO_SUBMIT_H

#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include "shrmem.h"
#include "schedulers.h"
#include "basic_types.h"
#include "util.h"

int parse_process_list(struct all_types **p_list, const size_t size, FILE* fp);


#endif
