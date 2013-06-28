#include "util.h"

void proc_pretty_printer (struct process proc) {
	printf("Process: \n");
	printf("exec_path: %s\n", proc.exec_path);
	printf("max_time: %lu\n", proc.max_time);
	printf("n_proc: %u\n", proc.n_proc);
	printf("argv: %s\n", proc.argv);
	printf("prev_index: %d\n", proc.prev_index);
	printf("next_index: %d\n", proc.next_index);
	printf("status: %s\n", proc.status?"pending":"running");
	printf("\n");
}

void sem_op (int idsem_free_proc, int n) {
	sem_op_s.sem_num = 0;
	sem_op_s.sem_op = n;
	sem_op_s.sem_flg = 0;
	if ( semop(idsem_free_proc, &sem_op_s, 1) < 0)
		printf("error: %s\n", strerror(errno));
}

