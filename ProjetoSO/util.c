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

/*
void proc_pretty_printer (struct process proc) {

	printf("%-8d", proc.n_req);
	printf("%-12s", sec2str(proc.max_time));
	printf("%-10u", proc.n_proc);
	// TODO: Update the line below to print current time.
	printf("%-12s", sec2str(proc.max_time));
	printf("%-12s", proc.status ? "PENDING" : "RUNNING");
	printf("%-s", strcat(strcat(proc.exec_name, " "), proc.argv));
	printf("\n");
}
*/

void proc_pretty_printer(struct all_types proc) {
	printf("%-8d", proc.flex_types.p.n_req);
	printf("%-12s", sec2str(proc.flex_types.p.max_time));
	printf("%-10u", proc.flex_types.p.n_proc);
	// TODO: Update the line below to print current time.
	printf("%-12s", sec2str(proc.flex_types.p.start_sec?(time(NULL) - proc.flex_types.p.start_sec):0));
	printf("%-12s", proc.flex_types.p.status ? "PENDING" : "RUNNING");
	printf("%-s", strcat(strcat(proc.flex_types.p.exec_name, " "), proc.flex_types.p.argv));
	printf("\n");

	// printf("Process: \n");
	// printf("exec_path: %s\n", proc.flex_types.p.exec_path);
	// printf("max_time: %lu\n", proc.flex_types.p.max_time);
	// printf("n_proc: %u\n", proc.flex_types.p.n_proc);
	// printf("argv: %s\n", proc.flex_types.p.argv);
	// printf("prev_index: %d\n", proc.prev_index);
	// printf("next_index: %d\n", proc.next_index);
	// printf("status: %s\n", proc.flex_types.p.status?"pending":"running");
	// printf("\n");
}

int sem_op (int idsem, int n) {
	int returnv;

	sem_op_s1.sem_num = 0;
	sem_op_s1.sem_op = n;
	sem_op_s1.sem_flg = 0;
	// what if quantum happens here? and if it came after the round table alert that executed sem_reset
	while ((returnv = semop(idsem, &sem_op_s1, 1)) < 0)
		if (errno != EINTR){
			printf("Semaphore error: %s\n", strerror(errno));
			exit(1);
		}
	return returnv;
}

int sem_op_sens (int idsem, int n) {
	sem_op_s2.sem_num = 0;
	sem_op_s2.sem_op = n;
	sem_op_s2.sem_flg = 0;
	// what if quantum happens here? and if it came after the round table alert that executed sem_reset
	return semop(idsem, &sem_op_s2, 1);
}

int sem_op_nblock (int idsem, int n) {
	sem_op_s3.sem_num = 0;
	sem_op_s3.sem_op = n;
	sem_op_s3.sem_flg = IPC_NOWAIT;
	return semop(idsem, &sem_op_s3, 1);
}

void sem_reset (int idsem) {
	while (sem_op_nblock(idsem, -1) >= 0);
}
