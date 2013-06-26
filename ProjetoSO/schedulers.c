#include "schedulers.h"

int large_quick_proc_schd (int np, int max_time) {
	return (int) (pow(np, 2) + (1 / ((float) max_time)));
}

int sjf_schd (int np, int max_time) {
	return (int) (pow(np, -2) + pow(max_time, -3));
}

int large_slow_proc_schd (int np, int max_time) {
	return (int) (pow(np, 2) + pow(max_time, 4));
}