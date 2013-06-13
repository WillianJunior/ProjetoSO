#include <stdio.h>
#include <stdlib.h>

#include "so_submit.h"

typedef struct process process;

int main(int argc, char *argv[]) {

    FILE *fp;
    struct process x;

    if(argc < 2) {
        fprintf(stderr, "Usage: so_submit <process file>.\n");
        exit(1);
    }


    

    return 0;
}
