#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "basic_types.h"
#include "spawner.h"

int main () {

	int idqueue;

	if (idqueue = msgget(MSGQ_KEY, 0x1FF) < 0) {
		printf( "erro na obtencao da fila\n" );
	}

	process proc;
	strcpy(proc.exec_name, "helloworld");
	strcpy(proc.exec_path, "helloworld");
	proc.max_time = 15;
	proc.n_proc = 4;
	strcpy(proc.argv, "");
	proc.priority_coef = 15.0;
	proc.in_use = 1;

	msgsnd(idqueue, &proc, sizeof(process), 0);

	return 0;

}