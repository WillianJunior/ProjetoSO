#include "so_list.h"

void print_table_header() {
	printf("%-8s", "Nreq");
	printf("%-12s", "Max_Time");
	printf("%-10s", "N_proc");
	printf("%-12s", "Time");
	printf("%-12s", "Status");
	printf("%-s", "Program");
	printf("\n");
}

void print_process_list() {
	process* p;
	p = get_first_proc();
	if(!p) {
		printf("The processes' table is empty. Try running 'so_submit <process file>' first.\n");
		return;
	}

	print_table_header();
    while(p) {
        proc_pretty_printer(*p);
        p = next_proc(p);
    }
}

int main(int argc, char *argv[]) {

	init();

	print_process_list();

	return 0;
}
