#include "so_schedule.h"

int main(int argc, char const *argv[]) {
	
	int free_proc;
	int status;
	int pid_fp;
	int pid_np;

	if (argc != 2) {
		printf("Usage: so_schedule <max_proc>\n");
		exit(1);
	}

	// set the number of free processes
	free_proc = atoi(argv[1])
	semop(free_proc);

	while (1) {
		// if there is at least one free process it won't block
		semop(-1);

		// obs: for the first version the scheduler will be aways executing
		// run through the process list and recover a new process that is:
		// status == PENDING
		// n_proc <= free_proc
		if (foundit) {
			// alocate the processes
			free_proc -= n_proc;
			semop(1 - n_proc);

			// change the process state
			proc.status = status.RUNNING;

			// send it to the spawner to be executed
			msgsnd(proc);

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
			wait(status);

			// kill and wait the other daemon
			if (status)
				kill(pid_fp, SIGKILL);
			else
				kill(pid_np, SIGKILL);

			// wait for the killed daemon
			wait(status);

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