#include "util.h"

void proc_pretty_printer(struct process proc) {

	printf("exec_path: %s\n", proc.exec_path);
	printf("max_time: %ul\n", max_time);
	printf("n_proc: %ud\n", n_proc);
	printf("argv: %s\n", argv);

}