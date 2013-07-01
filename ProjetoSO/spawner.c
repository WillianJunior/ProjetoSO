#include "spawner.h"

int pid;

int main(int argc, char const *argv[]) {
	
	int idsem_free_proc;
	int idqueue;
	int state;
	int *proc_index;
	all_types *proc;
	//all_types *proc_temp;

	// start the semaphore
	if ((idsem_free_proc = semget(FREE_PROC_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// access the msg queue from the spawner
	if ((idqueue = msgget(SCH_SPW_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		printf( "erro na obtencao da fila\n" );
		exit(1);
	}

	// create the zombie killer 
	signal(SIGALRM, zombie_killer);
	alarm(ZOMBIE_KILLER_TIMEOUT);

	init(PROC_TABLE_SHM_KEY);

	proc_index = malloc(sizeof(int)); // really need this!!

	while (1) {
		// get the all_types request from the message queue
		printf("[Breeder] Waiting a new all_types...\n");
		// warning: can receive signal from round_table alarm, but the while treat it
		while (msgrcv(idqueue, proc_index, sizeof(int), 0, 0) < 0)
			if (errno != EINTR){
				printf("[Breeder] Message Queue error: %s\n", strerror(errno));
				exit(1);
			}

		printf("process index: %d\n", *proc_index);
		proc = get_proc_by_index(*proc_index);

		printf("[Breeder] all_types received\n");
		proc_pretty_printer(*proc);

		// fork himself to a wrapper to set and treat the timeout
		if ((pid = fork()) == 0) {
			printf("[Wrapper] Wrapper created\n");

			// fork the wrapper to execute the all_types
			if ((pid = fork()) == 0) {
				// execute the all_types from the fork of the wrapper
				printf("[Executer] Now will execute the new program\n");
				execl(proc->flex_types.p.exec_path, proc->flex_types.p.exec_name, proc->flex_types.p.argv, (char *) 0);
			}

			// set timeout
			printf("[Wrapper] Setting timeout alarm\n");
			signal(SIGALRM, proc_killer);
			alarm(proc->flex_types.p.max_time);

			// set the initial time of execution
			proc->flex_types.p.start_sec = time(NULL);

			// wait for the process to finish
			printf("[Wrapper] Waiting for the program to finish...\n");
			wait(&state);
			alarm(0);
			printf("[Wrapper] State: %d\n", state);
			printf("[Wrapper] Program finished\n");
			
			// remove the process from the process table
			//proc->flex_types.p.status = FINISHED;
			remove_proc_shr_mem(proc);

			// send the signal of free all_types
			sem_op(idsem_free_proc, proc->flex_types.p.n_proc);

			return state;
		}
	}
	return 0;
}

void zombie_killer() {
	int removed;
	int state;

	printf("[Zombie Killer] Let's kill some zombies!!\n");
	removed = waitpid(-1, &state, WNOHANG);
	while (removed > 0) {
		printf("[Zombie Killer] Killed zombie all_types %d\n", removed);
		removed = waitpid(-1, &state, WNOHANG);
	}

	// reset the zombie killer timeout
	alarm(ZOMBIE_KILLER_TIMEOUT);
}

void proc_killer () {
	printf("[all_types Killer] forced kill on all_types %d\n", pid);
	kill(pid, SIGKILL);
}