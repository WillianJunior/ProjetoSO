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
#define PROC_TABLE_SHM_KEY 0x1927
#define COEF_LIST_1_SHM_KEY 0x1929
#define COEF_LIST_2_SHM_KEY 0x1931

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
	enum status status;
	int prev_index;
	int next_index;
	struct process* prev;
	struct process* next;
};

struct flex_process {
	char exec_name[PROC_EXEC_NAME_SIZE];
	char exec_path[PROC_EXEC_PATH_SIZE];
	unsigned long max_time;
	unsigned int n_proc;
	char argv[PROC_ARGV_SIZE];
	float priority_coef;
	enum status status;
};

struct priority_list {
	int proc_index;
	int priority_coef;
};

union all_types {
	int prev_index;
	int next_index;
	union all_types* prev;
	union all_types* next;
	struct flex_process p;
	struct priority_list pl;
};

typedef struct process process;
typedef struct flex_process flex_process;
typedef struct priority_list priority_list;
typedef union all_types all_types;

struct sembuf sem_op_s;

#endif
