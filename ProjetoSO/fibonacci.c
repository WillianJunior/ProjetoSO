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

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    int i;

    if(argc < 2) {
        printf("\n\nHey, come on... I need some parameters! :(\n\n");
        exit(1);
    }

    printf("\n\nI'm dummy and don't know what to do with these parameters:\n");

    for(i=0; i<argc; i++) {
        printf("argv[%d] = \t%s\n", i, argv[i]);
    }

    printf("\n");

    return 0;
}
    
