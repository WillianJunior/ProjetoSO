#include "flex_so_schedule.h"

int idsem_free_proc;
int idsem_sch_submit;
int idqueue;
int least_proc;
int pending;

int main(int argc, char const *argv[]) {
	
	int status;
	int pid_fp;
	int pid_np;
	int found = 0;
	all_types *proc;

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

	// get a communication channel between the scheduler and so_submit using a semaphore
	if ((idsem_sch_submit = semget(SCH_SBMT_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// access the msg queue from the spawner
	if ((idqueue = msgget(SCH_SPW_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		printf( "Error obtaining the msg queue: %s\n", strerror(errno));
		exit(1);
	}

	// set the number of free all_typeses
	sem_op(idsem_free_proc, atoi(argv[1]));

	// set the daemons waiters
	signal(SIGUSR1, freed_proc_daemon);
	signal(SIGUSR2, new_submit_daemon);

	while (1) {
		// if there is at least one free all_types it won't block
		sem_op(idsem_free_proc, -1);
		printf("-------------------------------------------------------\n");

		// run through the all_types list and recover a new all_types
		printf("Searching for a all_types...\n");
		least_proc = 100000; // large num
		pending = 0;
		if ((proc = get_first_proc()) != 0) {
			do {
				if (proc->p.n_proc < least_proc && proc->p.status == PENDING)
					least_proc = proc->p.n_proc;
				// search
				if (proc->p.status == PENDING) {
					pending = 1;
					if (proc->p.n_proc <= (semctl(idsem_free_proc, 0, GETVAL)+1)) {
						// change the all_types state
						proc->p.status = RUNNING;
						printf("Found!!!\n");
						proc_pretty_printer(*proc);
						found = 1;
						break;
					}
				}
			} while((proc = next_proc(proc)) != 0);
		} else
			printf("There isn't any all_types\n");

		if (found) {
			// alocate the all_typeses
			if (proc->p.n_proc != 1)
				sem_op(idsem_free_proc, 1 - proc->p.n_proc);

			// send it to the spawner to be executed
			if(msgsnd(idqueue, proc, sizeof(all_types), 0) < 0)
				printf("Error sending all_types to be executed: %s\n", strerror(errno));
			found = 0;
			printf("Sent to be executed\n");

		} else {
			// wait for a new all_types to be added or a free all_types signal
			// create the freed all_types daemon
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
		printf("Freed all_types\n");
		exit(0);
	} else {
		printf("All all_types are already free\n");
		pause();
	}
}

void new_submit_daemon () {

	sem_op(idsem_sch_submit, 1);
	sem_op(idsem_sch_submit, 0);
	printf("New submit\n");
	exit(1);
}