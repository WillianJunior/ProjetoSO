#include "so_schedule.h"

int idsem_free_proc;
int idqueue;

int main(int argc, char const *argv[]) {
	
	int free_proc;
	int status;
	int pid_fp;
	int pid_np;
	int found = 0;
	int i;

	// dummy for testing
	process dummy_proc[3];

	strcpy(dummy_proc[0].exec_name, "helloworld");
	strcpy(dummy_proc[0].exec_path, "helloworld");
	dummy_proc[0].max_time = 120;
	dummy_proc[0].n_proc = 1;
	strcpy(dummy_proc[0].argv,"");
	dummy_proc[0].status = PENDING;

	strcpy(dummy_proc[1].exec_name, "helloworld");
	strcpy(dummy_proc[1].exec_path, "helloworld");
	dummy_proc[1].max_time = 120;
	dummy_proc[1].n_proc = 1;
	strcpy(dummy_proc[1].argv,"");
	dummy_proc[1].status = PENDING;

	strcpy(dummy_proc[2].exec_name, "helloworld");
	strcpy(dummy_proc[2].exec_path, "helloworld");
	dummy_proc[2].max_time = 120;
	dummy_proc[2].n_proc = 1;
	strcpy(dummy_proc[2].argv,"");
	dummy_proc[2].status = PENDING;

	process proc;

	// check the input parameters
	if (argc != 2) {
		printf("Usage: so_schedule <max_proc>\n");
		exit(1);
	}

	// start the semaphore
	if ((idsem_free_proc = semget(FREE_PROC_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("erro na criacao do semaforo\n"); 
		exit(1);
	}

	// access the msg queue from the spawner
	if ((idqueue = msgget(SCH_SPW_MSGQ_KEY, 0x1FF)) < 0) {
		printf( "erro na obtencao da fila\n" );
		exit(1);
	}


	// set the number of free processes
	free_proc = atoi(argv[1]);
	sem_op(idsem_free_proc, free_proc);

	while (1) {
		// if there is at least one free process it won't block
		sem_op(idsem_free_proc, -1);

		// run through the process list and recover a new process
		printf("Searching for a process...\n");
		for (i=0; i<3; i++) {
			proc = dummy_proc[i];
			// search
			if (proc.status == PENDING && proc.n_proc <= free_proc) {
				found = 1;
				break;
			}
		}

		if (found) {
			printf("Found!!!\n");
			proc_pretty_printer(proc);
			// alocate the processes
			free_proc -= proc.n_proc;
			if (proc.n_proc != 1)
				sem_op(idsem_free_proc, 1 - proc.n_proc);

			// change the process state
			proc.status = RUNNING;

			// send it to the spawner to be executed
			if(msgsnd(idqueue, &proc, sizeof(process), 0) < 0)
				printf("error: %s\n", strerror(errno));
			found = 0;
			printf("Sent to be executed\n");

		} else {
			// wait for a new process to be added or a free process signal
			// create the freed process daemon
			if ((pid_fp = fork()) == 0)
				pause();
			
			// create the new submit daemon
			if ((pid_np = fork()) == 0)
				pause();

			// start the daemons
			kill(pid_fp, SIGUSR1);
			kill(pid_np, SIGUSR2);

			// wait for the first daemon to return
			wait(&status);

			// kill and wait the other daemon
			if (status)
				kill(pid_fp, SIGKILL);
			else
				kill(pid_np, SIGKILL);

			// wait for the killed daemon
			wait(&status);

			// get the result

		}


	}

	return 0;
}

void freed_proc_daemon () {

	exit(0);

}

void new_submit_daemon () {

	exit(1);

}