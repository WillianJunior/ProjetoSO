#include "so_schedule.h"

int idsem_free_proc;
int idqueue;
int least_proc;

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
	dummy_proc[1].max_time = 50;
	dummy_proc[1].n_proc = 1;
	strcpy(dummy_proc[1].argv,"");
	dummy_proc[1].status = PENDING;

	strcpy(dummy_proc[2].exec_name, "helloworld");
	strcpy(dummy_proc[2].exec_path, "helloworld");
	dummy_proc[2].max_time = 30;
	dummy_proc[2].n_proc = 4;
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
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// access the msg queue from the spawner
	if ((idqueue = msgget(SCH_SPW_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		printf( "Error obtaining the msg queue: %s\n", strerror(errno));
		exit(1);
	}


	// set the number of free processes
	free_proc = atoi(argv[1]);
	sem_op(idsem_free_proc, free_proc);

	// set the sorting alarm
	signal(SIGALRM, sorter);
	alarm(SORTING_TIMEOUT);

	// set the daemons waiters
	signal(SIGUSR1, freed_proc_daemon);
	signal(SIGUSR2, new_submit_daemon);

	while (1) {
		// if there is at least one free process it won't block
		//getchar();
		sem_op(idsem_free_proc, -1);
		//getchar();
		printf("sem: %d\n", semctl(idsem_free_proc, 0, GETVAL));

		// run through the process list and recover a new process
		printf("Searching for a process...\n");
		least_proc = 100000; // large num
		for (i=0; i<3; i++) {
			proc = dummy_proc[i];
			if (proc.n_proc < least_proc && proc.status == PENDING)
				least_proc = proc.n_proc;
			// search
			if (proc.status == PENDING && proc.n_proc <= semctl(idsem_free_proc, 0, GETVAL)-1) {
				// change the process state
				dummy_proc[i].status = RUNNING;
				printf("Found!!!\n");
				found = 1;
				break;
			}
		}

		if (found) {
			proc_pretty_printer(proc);
			// alocate the processes
			printf("sem: %d\n", semctl(idsem_free_proc, 0, GETVAL));
			printf("n_proc: %d\n", proc.n_proc);
			if (proc.n_proc != 1) {
				printf("hehre\n");
				sem_op(idsem_free_proc, 1 - proc.n_proc);
			}

			// send it to the spawner to be executed
			if(msgsnd(idqueue, &proc, sizeof(process), 0) < 0)
				printf("Error sending process to be executed: %s\n", strerror(errno));
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
			printf("Starting proc daemons\n");
			kill(pid_fp, SIGUSR1);
			kill(pid_np, SIGUSR2);

			// wait for the first daemon to return
			wait(&status);

			// kill and wait the other daemon
			if (WEXITSTATUS(status)) {
				kill(pid_fp, SIGKILL);
				printf("New submit\n");
			} else {
				kill(pid_np, SIGKILL);
				printf("Freed process\n");
			}

			// wait for the killed daemon
			wait(&status);

			// get the result
		}
	}

	return 0;
}

void freed_proc_daemon () {

	printf("least: %d\n", least_proc);
	printf("sem: %d\n", semctl(idsem_free_proc, 0, GETVAL));
	sem_op(idsem_free_proc, 1-least_proc);
	sem_op(idsem_free_proc, least_proc);
	printf("procfree\n");
	exit(0);

}

void new_submit_daemon () {

	pause();
	sleep(5000);
	printf("heeey\n");
	exit(1);

}

void sorter () {
	alarm(SORTING_TIMEOUT);
}