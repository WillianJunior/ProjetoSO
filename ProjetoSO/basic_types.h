#ifndef BASIC_TYPES
#define BASIC_TYPES

#define SHM_KEY 0x1927
#define SHM_BASE_PROC_NUMBER 50

#define PROC_EXEC_PATH_SIZE 100
#define PROC_ARGV_SIZE 100

struct process {
	char exec_path[PROC_EXEC_PATH_SIZE];
	unsigned long max_time;
	unsigned int n_proc;
	char argv[PROC_ARGV_SIZE];
};

#endif
