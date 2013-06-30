#include "util.h"

char* sec2str(unsigned long seconds) {
	unsigned long h, m, s;
	char hh[3], mm[3], ss[3], *str;
	ldiv_t res;

	res = ldiv(seconds, 3600);
	h = res.quot;
	res = ldiv(res.rem, 60);
	m = res.quot;
	s = res.rem;

	sprintf(hh, "%02lu", h);
	sprintf(mm, "%02lu", m);
	sprintf(ss, "%02lu", s);

	str = (char *) malloc(9 * sizeof(char));
	str[0] = '\0';
	strcat(str, hh);
	strcat(str, ":");
	strcat(str, mm);
	strcat(str, ":");
	strcat(str, ss);

	return str;
}

void proc_pretty_printer (struct process proc) {

	printf("%-8d", proc.n_req);
	printf("%-12s", sec2str(proc.max_time));
	printf("%-10u", proc.n_proc);
	// TODO: Update the line below to print current time.
	printf("%-12s", sec2str(proc.max_time));
	printf("%-12s", proc.status ? "PENDING" : "RUNNING");
	printf("%-s", strcat(strcat(proc.exec_name, " "), proc.argv));
	printf("\n");


	// printf("Process: \n");
	// printf("Nreq: %d\n", proc.n_req);
	// printf("exec_path: %s\n", proc.exec_path);
	// printf("max_time: %lu\n", proc.max_time);
	// printf("n_proc: %u\n", proc.n_proc);
	// printf("argv: %s\n", proc.argv);
	// printf("prev_index: %d\n", proc.prev_index);
	// printf("next_index: %d\n", proc.next_index);
	// printf("status: %s\n", proc.status? "PENDING" : "RUNNING");
	// printf("\n");
}
/*void proc_pretty_printer (union all_types proc) {
	printf("Process: \n");
	printf("exec_path: %s\n", proc.p.exec_path);
	printf("max_time: %lu\n", proc.p.max_time);
	printf("n_proc: %u\n", proc.p.n_proc);
	printf("argv: %s\n", proc.p.argv);
	printf("prev_index: %d\n", proc.prev_index);
	printf("next_index: %d\n", proc.next_index);
	printf("status: %s\n", proc.p.status?"pending":"running");
	printf("\n");
}*/

void sem_op (int idsem, int n) {
	sem_op_s.sem_num = 0;
	sem_op_s.sem_op = n;
	sem_op_s.sem_flg = 0;
	if ( semop(idsem, &sem_op_s, 1) < 0)
		printf("error: %s\n", strerror(errno));
}

int sem_op_nblock (int idsem, int n) {
	sem_op_s.sem_num = 0;
	sem_op_s.sem_op = n;
	sem_op_s.sem_flg = IPC_NOWAIT;
	return semop(idsem, &sem_op_s, 1);
}

void sem_reset (int idsem) {
	while (sem_op_nblock(idsem, -1) >= 0);
}