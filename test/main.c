#include <stdlib.h>
#include <stdio.h>

extern void fibonacci(long *vars);

enum {A, B, N, OUTPUT, T, Z, VAR_COUNT };
long vars[VAR_COUNT];

static void print_state(const char* title) {
    printf("%s\n", title);
    printf("a=%ld\n", vars[A]);
    printf("b=%ld\n", vars[B]);
    printf("n=%ld\n", vars[N]);
    printf("output=%ld\n", vars[OUTPUT]);
    printf("t=%ld\n", vars[T]);
    printf("x=%ld\n", vars[Z]);
}

int main(int argc, char **argv) {
    if (argc != VAR_COUNT + 1) {
        fprintf(stderr, "Usage: %s a b n output t z\n    (pass initial values in ALPHABETICAL order)\n:", argv[0]);
        return 1;
    }

    vars[A] = atol(argv[1]);
    vars[B] = atol(argv[2]);
    vars[N] = atol(argv[3]);
    vars[OUTPUT] = atol(argv[4]);
    vars[T] = atol(argv[5]);
    vars[Z] = atol(argv[6]);

    print_state("Initial state:");
    fibonacci(vars);
    print_state("Final state:");
    return 0;
}