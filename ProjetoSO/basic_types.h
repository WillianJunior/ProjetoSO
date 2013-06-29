#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <sys/sem.h>

#define SHM_BASE_PROC_NUMBER 50

#define PROC_EXEC_NAME_SIZE 50
#define PROC_EXEC_PATH_SIZE 100
#define PROC_ARGV_SIZE 100

#define FREE_PROC_SEM_KEY 0x1927
#define SCH_SBMT_SEM_KEY 0x1928
#define SCH_SPW_MSGQ_KEY 0x1927

enum status {
	RUNNING,
	PENDING,
	FINISHED
};

struct process {
	char exec_name[PROC_EXEC_NAME_SIZE];
	char exec_path[PROC_EXEC_PATH_SIZE];
	unsigned long max_time;
	unsigned int n_proc;
	char argv[PROC_ARGV_SIZE];
	float priority_coef;
	struct process* prev;
	struct process* next;
	int prev_index;
	int next_index;
	enum status status;
};

typedef struct process process;

struct sembuf sem_op_s;

#endif
