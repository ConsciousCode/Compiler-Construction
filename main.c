#define VAR_COUNT 4
#define LIVE_NAMES 
#define LIVE_OFFS 
#define PROGRAM_NAME add
// If you change this, run:
// scripts/build_main_str.sh < src/main.c > src/main.c.str

#include <stdlib.h>
#include <stdio.h>

#ifndef LIVE_COUNT
#define LIVE_COUNT 0
#endif

#ifndef VAR_COUNT
#define VAR_COUNT 0
#endif

#ifndef LIVE_NAMES
#define LIVE_NAMES "input"
#endif

#ifndef LIVE_OFFS
#define LIVE_OFFS
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME program 
#endif

extern long PROGRAM_NAME(long *vars);

static const char* live_names[] = {
    LIVE_NAMES, NULL
};

static const int live_offs[] = {
    LIVE_OFFS, -1
};

int main(int argc, char **argv) {
    long vars[VAR_COUNT] = {0};

    if (argc < 2) {
        fprintf(stderr, "Usage: %s", argv[0]);
        for(const char **it = live_names; *it != NULL; ++it) {
            fprintf(stderr, " %s", *it);
        }
        fprintf(stderr, "\n");
        return 1;
    }

    for (int i = 0; i < LIVE_COUNT; i++) {
        if(i + 1 > argc) {
            fprintf(stderr, "Error: Not enough args\n");
            return 1;
        }
        vars[live_offs[i]] = atol(argv[i + 1]);
    }

    printf("Initial state: %d\n", LIVE_COUNT);
    for (int i = 0; i < LIVE_COUNT; i++) {
        printf("  var[%s] = %ld\n", live_names[i], vars[live_offs[i]]);
    }

    long output = PROGRAM_NAME(vars);

    printf("\nFinal state:\n");
    printf("  var[output] = %ld\n", output);
    
    return 0;
}
