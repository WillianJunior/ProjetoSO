#include "so_schedule.h"

int idsem_free_proc;
int idqueue;
int least_proc;
int pending;
int i;

int main(int argc, char const *argv[]) {
	
	int status;
	int pid_fp;
	int pid_np;
	int found = 0;

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
	sem_op(idsem_free_proc, atoi(argv[1]));

	// set the sorting alarm
	signal(SIGALRM, sorter);
	alarm(SORTING_TIMEOUT);

	// set the daemons waiters
	signal(SIGUSR1, freed_proc_daemon);
	signal(SIGUSR2, new_submit_daemon);

	while (1) {
		// if there is at least one free process it won't block
		sem_op(idsem_free_proc, -1);
		printf("-------------------------------------------------------\n");

		// run through the process list and recover a new process
		printf("Searching for a process...\n");
		least_proc = 100000; // large num
		pending = 0;
		for (i=0; i<3; i++) {
			if (dummy_proc[i].n_proc < least_proc && dummy_proc[i].status == PENDING)
				least_proc = dummy_proc[i].n_proc;
			// search
			if (dummy_proc[i].status == PENDING) {
				pending = 1;
				if (dummy_proc[i].n_proc <= (semctl(idsem_free_proc, 0, GETVAL)+1)) {
					// change the process state
					dummy_proc[i].status = RUNNING;
					printf("Found!!!\n");
					proc_pretty_printer(dummy_proc[i]);
					found = 1;
					break;
				}
			}
		}

		if (found) {
			// alocate the processes
			if (dummy_proc[i].n_proc != 1)
				sem_op(idsem_free_proc, 1 - dummy_proc[i].n_proc);

			// send it to the spawner to be executed
			if(msgsnd(idqueue, &dummy_proc[i], sizeof(process), 0) < 0)
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
			if (status)
				kill(pid_fp, SIGKILL);
			else
				kill(pid_np, SIGKILL);

			// wait for the killed daemon
			wait(&status);
		}
	}

	return 0;
}

void freed_proc_daemon () {

	if (pending) {
		sem_op(idsem_free_proc, 1-least_proc);
		sem_op(idsem_free_proc, least_proc);
		printf("Freed process\n");
		exit(0);
	} else {
		printf("All process are already free\n");
		pause();
	}
}

void new_submit_daemon () {

	// todo
	pause();
	printf("New submit\n");
	exit(1);
}

void sorter () {
	// todo
	alarm(SORTING_TIMEOUT);
}