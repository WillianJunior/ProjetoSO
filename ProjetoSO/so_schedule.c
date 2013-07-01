#include "so_schedule.h"

int idsem_free_proc;
int idsem_sch_submit;
int idsem_esc_count;
int idsem_esc_crit;
int idqueue;
int least_proc;
int pending;
int scheduler = 0;

int main(int argc, char const *argv[]) {
	
	int status;
	int pid_fp;
	int pid_np;
	int found = 0;
	int old_free_count;
	int proc_index;
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

	// start the adaptative scheduler counter
	if ((idsem_esc_count = semget(ADPT_ESC_COUNT_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// start the adaptative scheduler critic session semaphore
	if ((idsem_esc_crit = semget(ADPT_ESC_CRIT_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// access the msg queue from the spawner
	if ((idqueue = msgget(SCH_SPW_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		printf( "Error obtaining the msg queue: %s\n", strerror(errno));
		exit(1);
	}

	// start the scheduler changer
	signal(SIGALRM, round_table);
	alarm(ROUND_TABLE_TIMEOUT);
	sem_op(idsem_esc_crit, 1);

	// attach the shared mem for all proc_shr functions
	init(COEF_LIST_1_SHM_KEY);

	// set the number of free processes
	sem_op(idsem_free_proc, atoi(argv[1]));

	// set the daemons waiters
	signal(SIGUSR1, freed_proc_daemon);
	signal(SIGUSR2, new_submit_daemon);
	//signal(SIGTERM, finalize);

	while (1) {
		// if there is at least one free process it won't block
		printf("sem = %d\n", semctl(idsem_free_proc, 0, GETVAL));
		sem_op(idsem_free_proc, -1); // warning: can receive signal from round_table alarm NEED TO BE TREATED PROPERLY!!!!!!!!!!!
		sem_op(idsem_free_proc, 1);// warning: can receive signal from round_table alarm NEED TO BE TREATED PROPERLY!!!!!!!!!!!
		printf("-------------------------------------------------------\n");

		// run through the process list and recover a new process
		printf("Searching for a process...\n");
		least_proc = 100000; // large num
		pending = 0;
		// semaphore to make sure that when the scheduler start to seek a proc it doesn't change to other scheduler list
		sem_op(idsem_esc_crit, -1);
		
		// var to make sure that no process have been freed while we run through the process list
		old_free_count = semctl(idsem_free_proc, 0, GETVAL);

		// try to find out a executable process
		if ((proc = get_first_proc()) != 0) {
			do {
				if (proc->flex_proc.pl.testp.n_proc < least_proc && proc->flex_proc.pl.testp.status == PENDING)
					least_proc = proc->flex_proc.pl.testp.n_proc;
				// search
				if (proc->flex_proc.pl.testp.status == PENDING) {
					pending = 1;
					// racing condition: a process can be freed while running throught the list: treated with old_free_count
					if (proc->flex_proc.pl.testp.n_proc <= semctl(idsem_free_proc, 0, GETVAL)) {
						// change the process state
						proc->flex_proc.pl.testp.status = RUNNING;
						printf("Found!!!\n");
						proc_index_test_pretty_printer(*proc);
						found = 1;
						break;
					}
				}
			} while((proc = next_proc(proc)) != 0);
		} else
			printf("There isn't any process\n");

		// unlock the round_table to change the scheduler in use
		sem_op(idsem_esc_crit, 1);

		if (found) {
			// increment the scheduler runned process counter
			sem_op(idsem_esc_count, 1);	

			// alocate the processes
			sem_op(idsem_free_proc, -proc->flex_proc.pl.testp.n_proc);

			// send it to the spawner to be executed
			proc_index = index_proc(proc);
			if(msgsnd(idqueue, &proc_index, sizeof(int), 0) < 0)
				printf("Error sending process to be executed: %s\n", strerror(errno));
			found = 0;
			printf("Sent to be executed\n");

		} else {
			// if the process list have been searched and the number of free process is still the same
			if (semctl(idsem_free_proc, 0, GETVAL) == old_free_count) {
				// wait for a new process to be added or a free process signal
				// create the freed process daemon
				if ((pid_fp = fork()) == 0)
					pause();
				
				// create the new submit daemon
				if ((pid_np = fork()) == 0)
					pause();

				// start the daemons
				printf("Starting free_process and new_submit listeners\n");
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
	}

	return 0;
}

void round_table () {
	
	printf("[Round Table] Let's change?\n");
	// check if it have already runned the minimum amount of processes
	if(sem_op_nblock(idsem_esc_count, -ROUND_TABLE_MIN_COUNT) < 0) {
		printf("[Round Table] Noooot...\n");
		alarm(ROUND_TABLE_TIMEOUT);
		return;
	}
	
	printf("[Round Table] Ok, time to change\n");
	// change the actual scheduler, only works for 2 scheduleres, for now
	if (scheduler) {
		scheduler = 0;
		init(COEF_LIST_1_SHM_KEY);
	} else {
		scheduler = 1;
		init(COEF_LIST_2_SHM_KEY);
	}
	sem_reset (idsem_esc_count);
	alarm(ROUND_TABLE_TIMEOUT);
}

void freed_proc_daemon () {

	if (pending) {
		sem_op(idsem_free_proc, -least_proc);
		sem_op(idsem_free_proc, least_proc);
		printf("Freed process\n");
		exit(0);
	} else {
		printf("All process are already free\n");
		pause();
	}
}

void new_submit_daemon () {

	sem_op(idsem_sch_submit, 1);
	sem_op(idsem_sch_submit, 0);
	printf("New submit\n");
	exit(1);
}