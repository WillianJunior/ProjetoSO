#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#define SHM_BASE_PROC_NUMBER 50
#define BITMAP_BLOCK_SIZE    sizeof(unsigned char) * 8
#define BITMAP_SIZE          SHM_BASE_PROC_NUMBER / BITMAP_BLOCK_SIZE

#define PROC_EXEC_NAME_SIZE 50
#define PROC_EXEC_PATH_SIZE 100
#define PROC_ARGV_SIZE 100

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
};

typedef struct process process;

#endif
