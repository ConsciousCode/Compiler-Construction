#include <stdlib.h>
#include <stdio.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME program 
#endif

extern void PROGRAM_NAME(long *vars);

#define MAX_VARS 16

int main(int argc, char **argv) {
    long vars[MAX_VARS] = {0};

    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s <var1> <var2> ... <varN>\n"
            "  Pass initial values in ALPHABETICAL order.\n"
            "  Example:\n"
            "    factorial: ./run 0 5 0 0   (output x y z)\n"
            "    fibonacci: ./run 0 1 10 0 0 0   (a b n output t z)\n"
            "    collatz:   ./run 10 0 0 0 0 0   (input n output quot rem steps)\n",
            argv[0]);
        return 1;
    }

    int var_count = argc -1;

    for (int i = 0; i < var_count; i++) {
        vars[i] = atol(argv[i + 1]);
    }

    printf("Initial state:\n");
    for (int i = 0; i < var_count; i++) {
        printf("  var[%d] = %ld\n", i, vars[i]);
    }

    PROGRAM_NAME(vars);

    printf("\nFinal state:\n");
    for (int i = 0; i < var_count; i++){
        printf("  var[%d] = %ld\n", i, vars[i]);
    }
    
    return 0;
}