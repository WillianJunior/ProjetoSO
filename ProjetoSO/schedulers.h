#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include <math.h>

/**********************************************/
/***				Schedulers				***/
/**********************************************/
/*** Quick processes with many needed proc	***/
/*** Shortest Job First 					***/
/*** Slow processes with many needed proc 	***/
/**********************************************/

int large_quick_proc_schd (int np, int max_time);
int sjf_schd (int np, int max_time);
int large_slow_proc_schd (int np, int max_time);

#endif