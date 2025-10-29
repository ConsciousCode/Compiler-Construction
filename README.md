## Project 1: Simple Code Generation
> **Team: Ctrl+Alt+Deletes**
> 
> Nicholas Livingstone, Robert McDaniels, Key Vollers

A compiler for the `.while` language that generates RISC-V assembly code.

## Prerequisites

- CMake (version 3.15 or higher)
- GCC with RISC-V support

## Building
```bash
cmake -S . -B build
cmake --build build -j
```

## Quick Start

1. Compile a `.while` source file to RISC-V assembly:
```bash
wisc <input_file>
```

2. Run the generated assembly on a RISC-V machine (see examples below)

## Running Examples

Each example requires specific variable initialization. Variables are passed as command-line arguments in the order specified.

### Fibonacci
Computes Fibonacci sequence.

**Variables:** `a, b, n, output, t, z`
```bash
gcc fibonacci.s ./test/main.c -DPROGRAM_NAME=fibonacci -o run_fib
./run_fib 0 1 10 0 0 0
```

### Factorial
Computes factorial of a number.

**Variables:** `output, x, y, z`
```bash
gcc factorial.s ./test/main.c -DPROGRAM_NAME=factorial -o run_fact
./run_fact 0 5 0 0
```

### Collatz Conjecture
**Variables:** `a, b, n, output, t, z`
```bash
gcc collatz.s ./test/main.c -DPROGRAM_NAME=collatz -o run_clz
./run_clz 10 0 0 0 0 0
```

### Stress Test: Factorial
**Variables:** `input, i, output, t, x, y, z`
```bash
gcc stress_factorial.s ./test/main.c -DPROGRAM_NAME=stress_factorial -o run_sfact
./run_sfact 0 5 0 0 0 0 0
```

## Development Tools

### Scanner Example
Outputs tokens for a given `.while` file.
```bash
scanner_example <input_file>
```

### Scanner Tests
Run scanner test suite using Google Test:
```bash
scanner_test
```

Or using CTest:
```bash
ctest --output-on-failure
```

### Parser Example
Parses a `.while` program and outputs:
- Printed parse tree
- Printed DOT data for AST visualization
- DOT file for parse tree visualization
```bash
parser_example <input_file>
```

## Project Structure

```
Compiler-Project-1                                 
├─ .clang-format                                   
├─ CMakeLists.txt                                  
├─ README.md                                       
├─ examples/                                       // Contains example while programs
├─ grammar.txt                                     // Grammar for developer reference
├─ src/                                            // Source Code
└─ test/                                           // Contains binary assembly files for some example while programs
```