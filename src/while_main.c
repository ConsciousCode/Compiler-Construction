/**
 * This is a drop-in harness for running While programs.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

// Array of program strings linked in assembly
extern const char *varnames[];

// Instead of looking for a sentinel at runtime we could link this directly
//extern int nvars;

// Assembly's main program function
extern void program(int64_t *vars);

int main(int argc, char *argv[]) {
    // Alternatively, could use extern int nvars too
    int nvars = 0;
    for(; varnames[nvars] != NULL; ++nvars) {}
    
    if(argc != 1 + nvars) {
        fprintf(stderr, "Usage: %s", argv[0]);
        for(int i = 0; i < nvars; ++i) {
            fprintf(stderr, " %s", varnames[i]);
        }
        fprintf(stderr, "\n");
        return 1;
    }
    
    // Array of variable values
    int64_t vars[nvars];
    
    // Read in all the variables so we can error before printing the
    // initial state
    for(int i = 0; i < nvars; ++i) {
        const char *start = argv[i+1];
        char *end;
        vars[i] = strtoll(start, &end, 10);
        if(end == start || *end != '\0') {
            if(errno == ERANGE) {
                fprintf(stderr, "Argument %s='%s' out of range\n",
                    varnames[i], argv[i+1]
                );
            } else {
                fprintf(stderr, "Invalid argument %s='%s'\n",
                    varnames[i], argv[i+1]
                );
            }
            return 1;
        }
    }
    
    printf("Initial state:\n");
    for(int i = 0; i < nvars; ++i) {
        printf("%s=%ld\n", varnames[i], vars[i]);
    }
    
    program(vars);
    
    printf("Final state:\n");
    for(int i = 0; i < nvars; ++i) {
        printf("%s=%ld\n", varnames[i], vars[i]);
    }
    
    return 0;
}
