#include "so_shutdown.h"

int main () {
	
	int idqueue_shutdown;
	int idsem_free_proc;

	int *spawner_pid = malloc(sizeof(int));
	int *scheduler_pid = malloc(sizeof(int));
	int *nproc = malloc(sizeof(int));

	if ((idqueue_shutdown = msgget(SHTDWN_PIDS_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		printf( "erro na obtencao da fila\n" );
		exit(1);
	}

	// start the semaphore
	if ((idsem_free_proc = semget(FREE_PROC_SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("Error obtaining the semaphore: %s\n", strerror(errno)); 
		exit(1);
	}

	// stop the scheduler from spawning more processes and the spawner from running more processes from its queue (SIGTERM)
	msgrcv(idqueue_shutdown, spawner_pid, sizeof(int), 0, 0);
	kill(*spawner_pid, SIGTERM);
	msgrcv(idqueue_shutdown, scheduler_pid, sizeof(int), 0, 0);
	kill(*scheduler_pid, SIGTERM);

	// begin countdown to force finish
	alarm(SHUTDOWN_TIMEOUT);
	signal(SIGALRM, hard_shutdown);

	// wait for the semaphore of process. if all processes are free then all processes have stoped
	msgrcv(idqueue_shutdown, nproc, sizeof(int), 0, 0);
	sem_op_sens(idsem_free_proc, -(*nproc));

	
	// disarm the alarm
	printf("That was close!! %d seconds left to a forced shutdown\n", alarm(0));
	
	// if not send SIGKILL to every live process

	// kill the spawner and the scheduler
	kill(*spawner_pid, SIGKILL);
	kill(*scheduler_pid, SIGKILL);

	// finaly kill itself
	return 0;
}

void hard_shutdown () {

	all_types *proc;
	all_types *aux;

	// kill all RUNNING processes
	init(PROC_TABLE_SHM_KEY);

	proc = get_first_proc();
	while (proc) {
		if (proc->flex_types.p.status == RUNNING) {
			printf("Gonna kill process: \n");
			proc_pretty_printer(*proc);
			aux = proc;
			proc = next_proc(proc);
			kill(aux->flex_types.p.pid, SIGKILL);
		} else
			proc = next_proc(proc);
	}
}