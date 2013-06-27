#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#define SHM_BASE_PROC_NUMBER 50

#define PROC_EXEC_NAME_SIZE 50
#define PROC_EXEC_PATH_SIZE 100
#define PROC_ARGV_SIZE 100

enum status {
	RUNNING,
	PENDING
};

struct process {
	char exec_name[PROC_EXEC_NAME_SIZE];
	char exec_path[PROC_EXEC_PATH_SIZE];
	unsigned long max_time;
	unsigned int n_proc;
	char argv[PROC_ARGV_SIZE];
	float priority_coef;
	enum status status;
};

typedef struct process process;

#endif
