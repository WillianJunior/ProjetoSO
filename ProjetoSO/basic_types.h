#ifndef BASIC_TYPES
#define BASIC_TYPES

struct process {
	char* exec_path;
	int argc;
	char* argv[];
	unsigned long max_time;
	unsigned int n_proc
};

#endif