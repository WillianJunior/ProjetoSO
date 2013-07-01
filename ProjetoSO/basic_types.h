#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <sys/sem.h>

#define SHM_BASE_PROC_NUMBER 50

#define SHUTDOWN_TIMEOUT 30

#define PROC_EXEC_NAME_SIZE 50
#define PROC_EXEC_PATH_SIZE 100
#define PROC_ARGV_SIZE 100

#define ROUND_TABLE_TIMEOUT 10
#define ROUND_TABLE_MIN_COUNT 3

#define FREE_PROC_SEM_KEY 0x1927
#define SCH_SBMT_SEM_KEY 0x1928
#define ADPT_ESC_COUNT_SEM_KEY 0x1929
#define ADPT_ESC_CRIT_SEM_KEY 0x192A
#define PROC_TABLE_MUTEX_SEM_KEY 0x192B

#define PROC_TABLE_SHM_KEY 0x1927
#define COEF_LIST_1_SHM_KEY 0x1929
#define COEF_LIST_2_SHM_KEY 0x192B

#define SCH_SPW_MSGQ_KEY 0x1927

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
	int n_req;
	int start_sec;
	int pid;
	int sjf_sch_index;
	int ljf_sch_index;
};

struct priority_list {
	int proc_index;
	int priority_coef;
};

struct all_types {
	union flex_types {
		struct process p;
		struct priority_list pl;
	} flex_types;
	int prev_index;
	int next_index;
	struct all_types* prev;
	struct all_types* next;
};

typedef struct process process;
typedef struct priority_list priority_list;
typedef struct all_types all_types;

struct sembuf sem_op_s1;
struct sembuf sem_op_s2;

#endif
