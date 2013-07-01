/**
 * Universidade de Brasília - UnB
 * Alunos: Alexandre Lucchesi Alencar - 09/0104471
 *         Willian Júnior - 09/0135806
 *
 * Sistemas Operacionais - Profa. Alba Melo
 *
 * Trabalho 1
 *
 * This code can be download in: https://github.com/WillianJunior/ProjetoSO.git
 *
 */

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
    int i, n_proc;

	init(PROC_TABLE_SHM_KEY);

	p = get_first_proc();
	if(!p) {
		printf("The processes' table is empty. Try running 'so_submit <process file>' first.\n");
		return;
	}

	print_table_header();
    while(p) {
        proc_pretty_printer(*p);
        n_proc = p->flex_types.p.n_proc;
        for(i=0; i<n_proc; i++) {
            p = next_proc(p);
        }
    }
}

void print_process_index_list(int key) {
	all_types* index;
	all_types* p;
    int i, n_proc;

	init(key);
	index = get_first_proc();
	if(!index)
		return;

	print_table_header();
    while(index) {
    	init(PROC_TABLE_SHM_KEY);
        p = get_proc_by_index(index->flex_types.pl.proc_index);
        proc_pretty_printer(*p);
        n_proc = p->flex_types.p.n_proc;
        init(key);
        for(i=0; i<n_proc; i++) {
            index = next_proc(index);
        }
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
