#include "so_list.h"

void print_process_list() {
	process* p;
	p = get_first_proc();
    while(p) {
        proc_pretty_printer(*p);
        p = next_proc(p);
    }
}

int main() {

	print_process_list();

	return 0;
}

