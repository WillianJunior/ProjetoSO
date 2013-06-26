#include "spawner.h"

int main(int argc, char const *argv[]) {
	
	int idqueue;
	int pid;
	int state;
	process proc;

	if (idqueue = msgget(MSGQ_KEY, IPC_CREAT|0x1FF) < 0) {
		printf( "erro na obtencao da fila\n" );
		exit(1);
	}

	while (1) {
		// get the process request from the message queue
		printf("Waiting a new process...\n");
		if(msgrcv(idqueue, &proc, sizeof(process), 0, 0) < 0) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		printf("Process received\n");

		// fork himself to a wrapper to set and treat the timeout
		if ((pid = fork()) == 0) {
			printf("Wrapper created\n");
			// fork the wrapper to execute the process
			if ((pid = fork()) == 0) {
				// execute the process from the fork of the wrapper
				printf("Now will execute the new programm: \n");
				proc_pretty_printer(proc);
				execl(proc.exec_path, proc.exec_name, proc.argv, (char *) 0);
				return 0;
			}

			// set timeout
			printf("Setting timeout alarm\n");
			signal(SIGALRM, func(pid));
			alarm(proc.max_time);
			printf("Waiting for the programm to finish...\n");
			wait(&state);
			alarm(0);
			printf("Programm finished\n");
			return state;
		}
	}

	return 0;
}

void *func (int pid) {
	printf("forced kill on process: %d\n", pid );
	kill(SIGKILL, pid);
}