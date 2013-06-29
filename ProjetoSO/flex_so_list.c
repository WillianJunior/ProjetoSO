#include "flex_so_list.h"

void print_process_list() {
	all_types* p;
	p = get_first_proc(PROC_TABLE_SHM_KEY);
    while(p) {
        proc_pretty_printer(*p);
        p = next_proc(p);
    }
}

int main() {

	print_process_list();

	return 0;
}

