#ifndef BASIC_TYPES
#define BASIC_TYPES

#define SHM_KEY 0x1927
#define SHM_BASE_PROC_NUMBER 50

struct process {
	char *exec_path;
	int argc;
	unsigned long max_time;
	unsigned int n_proc;
	char **argv;
};

#endif
