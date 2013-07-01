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
	all_types* p;

	init(PROC_TABLE_SHM_KEY);

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

void print_process_index_list(int key) {
	all_types* p;

	init(key);

	p = get_first_proc();
	if(!p)
		return;

	print_table_header();
    while(p) {
        proc_index_test_pretty_printer(*p);
        p = next_proc(p);
    }
}


int main(int argc, char *argv[]) {

	printf("Process Table\n");
	print_process_list();
	printf("\n\nProcess Table ordered by SJF\n");
	print_process_index_list(COEF_LIST_1_SHM_KEY);
	printf("\n\nProcess Table ordered by LJF\n");
	print_process_index_list(COEF_LIST_2_SHM_KEY);
	//print_process_index_list();

	return 0;
}
