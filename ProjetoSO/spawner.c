#include "spawner.h"

int pid;
int zombie_killer_init = 1;
int id_zombie_sem;
struct sembuf sem_op;

int main(int argc, char const *argv[]) {
	
	int idqueue;
	int state;
	process proc;


	if ((id_zombie_sem = semget(SEM_KEY, 1, IPC_CREAT|0x1ff)) < 0) { 
		printf("erro na criacao do semaforo\n"); 
		exit(1);
	}


	if (idqueue = msgget(MSGQ_KEY, IPC_CREAT|0x1FF) < 0) {
		printf( "erro na obtencao da fila\n" );
		exit(1);
	}

	while (1) {
		// get the process request from the message queue
		printf("[Breeder] Waiting a new process...\n");
		while (msgrcv(idqueue, &proc, sizeof(process), 0, 0) < 0)
			printf("error: %s\n", strerror(errno));

		printf("[Breeder] Process received\n");

		// fork himself to a wrapper to set and treat the timeout
		if ((pid = fork()) == 0) {
			printf("[Wrapper] Wrapper created\n");
			// fork the wrapper to execute the process
			if ((pid = fork()) == 0) {
				// execute the process from the fork of the wrapper
				printf("[Executer] Now will execute the new programm: \n");
				proc_pretty_printer(proc);
				execl(proc.exec_path, proc.exec_name, proc.argv, (char *) 0);
			}

			// set timeout
			printf("[Wrapper] Setting timeout alarm\n");
			signal(SIGALRM, proc_killer);
			alarm(proc.max_time);
			printf("[Wrapper] Waiting for the programm to finish...\n");
			wait(&state);
			printf("[Wrapper] State: %d\n", state);
			alarm(0);
			printf("[Wrapper] Programm finished\n");
			return state;
		}

		// start the zombie killer if it is asleep
		if (zombie_killer_init) {
			signal(SIGALRM, zombie_killer);
			alarm(ZOMBIE_KILLER_TIMEOUT);
			zombie_killer_init = 0;
		}
	}

	return 0;
}

void check_zero_sem () {
	sem_op.sem_num = 0;
	sem_op.sem_op = 0;
	sem_op.sem_flg = 0;
	if ( semop(id_zombie_sem, &sem_op, 1) < 0)
		printf("erro no p=%d\n", errno);
}

void p_sem () {
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	if ( semop(id_zombie_sem, &sem_op, 1) < 0)
		printf("erro no p=%d\n", errno);
}

void v_sem () {
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	if ( semop(id_zombie_sem, &sem_op, 1) < 0)
		printf("erro no p=%d\n", errno);
}

void zombie_killer() {
	// using this semaphore we avoid the case where the zombie killer kill a zombie grandchild near the wait in line 50
	// is it possible?
	// optmize the zombie killer to only call it when there is a zombie process 
	p_sem();
	int removed;
	int state;
	printf("[Zombie Killer] Let's kill some zombies!!\n");
	do {
		if ((removed = waitpid(-1, &state, WNOHANG)) > 0)
			printf("[Zombie Killer] Killed zombie process %d\n", removed);
	} while (removed > 0);

	v_sem();

	// reset the zombie killer timeout
	alarm(ZOMBIE_KILLER_TIMEOUT);
}

void proc_killer () {
	check_zero_sem();
	printf("[Process Killer] forced kill -9 on process %d\n", pid);
	kill(pid, SIGKILL);
}