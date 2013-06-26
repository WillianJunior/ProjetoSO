#include <stdio.h>

#include "util.h"

void proc_pretty_printer(struct process proc) {

	printf("Process: \n");
	printf("exec_path: %s\n", proc.exec_name);
	printf("exec_path: %s\n", proc.exec_path);
	printf("max_time: %lu\n", proc.max_time);
	printf("n_proc: %u\n", proc.n_proc);
	printf("argv: %s\n", proc.argv);

}
