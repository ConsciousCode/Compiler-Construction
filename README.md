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
## Command Line Reference for `wisc`

The `wisc` compiler supports multiple output modes and debugging tools for viewing intermediate compilation stages.
### Basic usage

`wisc <input_file> [-o <output_file>]`

By default, `wisc` generates RISC-V assembly and writes it to `out.s`.

---
### Stage-inspection Flags

|Flag|Short|Description|
|---|---|---|
|`--print-tokens`|`-pt`|Print lexical tokens from the scanner.|
|`--print-parse-tree`|`-ppt`|Print a human-readable parse tree.|
|`--graph-parse-tree [file]`|`-gpt`|Output a Graphviz DOT parse tree (default: `parse_tree.dot`).|
|`--print-ast`|`-pa`|Print the AST in a debug textual form.|
|`--graph-ast [file]`|`-ga`|Output a Graphviz DOT AST (default: `ast.dot`).|
|`--graph-cfg [file]`|`-gc`|Output a Graphviz DOT CFG (default: `cfg.dot`).|

Multiple flags may be combined.

---
### Output Flags

|Flag|Short|Description|
|---|---|---|
|`--output <file>`|`-o`|Write RISC-V assembly to `<file>` (default: `out.s`).|

Examples:
```bash
# Generate assembly to custom file
wisc examples/fib.while -o fibonacci.s

# Generate AST graph to custom file
wisc examples/fib.while -ga fib_ast.dot

# Generate parse tree graph (uses default parse_tree.dot)
wisc examples/fib.while -gpt
```

---
### CFG Visualization Options

|Flag|Short|Description|
|---|---|---|
|`--show-cfg-vars`|`-sv`|Include the list of used variables in each CFG block.|

Example:
```bash
wisc examples/loop.while -gc loop_cfg.dot -sv
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
### Equivalent C program
Parses a `.while` program and outputs an equivalent C program. This is intended to help verify the correctness of codegen from the backend.
```bash
cgen <input_file>
```

Example input:
```
while n > 0 do
  output := output + 1;
  n := n - 1
od
```

Example output:
```c
#include <stdint.h>
void loop(int64_t vars[]) {
    int64_t n = vars[0], output = vars[1];
    while ((n > 0)) {
        output = (output + 1);
        n = (n - 1);
    }
    vars[0] = n;
    vars[1] = output;
}
```

### While program annotation
Parses a `.while` program and outputs an equivalent program with nodes annotated with labels. This serves as a debugging aid to verify the AST and identify which nodes are being labeled. Specifically, `:=`, `skip`, and the root condition nodes of `if` and `while` commands.
```bash
whilegen <input_file>
```

Example input:
```
while n > 0 do
  output := output + 1;
  n := n - 1
od
```

Example output:
```
while [n > 0] do {- 0 -}
    output := (output + 1) {- 1 -}
    n := (n - 1) {- 2 -}
od
```

## Benchmark Suite

A benchmark script is provided to test and compare the performance of RISC-V assembly output against the C-generated equivalent programs.

### Running Benchmarks
```bash
./scripts/benchmark_while.sh [build_dir]
```

**Prerequisites:**
- For non-RISC-V systems: `qemu-user` package must be installed
- All WHILE programs must be built first (`cmake --build build`)

**Example:**
```bash
# Build all programs
cmake --build build --target while_programs

# Run benchmark suite
./scripts/benchmark_while.sh build
```

### Benchmark Output

The script runs a comprehensive test suite covering all example programs with various input values. Output is displayed in a clean table format:
```
================================================================================
                        WHILE Program Benchmark Suite
================================================================================

Program              | Input      | RISC-V Time  | C Time       | Output Match
---------------------------------------------------------------------------
fibonacci            | 10         | 0m0.003s     | 0m0.001s     | ✓
fibonacci            | 40         | 0m0.004s     | 0m0.001s     | ✓
factorial            | 5          | 0m0.005s     | 0m0.001s     | ✓
collatz              | 27         | 0m0.003s     | 0m0.001s     | ✓
stress_factorial     | 12         | 2m2.220s     | 2m1.850s     | ✓
...
```

**Status indicators:**
- ✓ - Both versions produce identical output
- ✗ - Output mismatch detected (indicates a potential codegen issue)

## Performance Results
Current timing results on different programs.
Output match compares the c program to the release 2 program to verify correctness.

| Program          | Input | Release 1 (s) | Release 2 (s) | C Time (s) | Output Match |
|------------------|-------|---------------|---------------|------------|--------------|
| fibonacci        | 10    | 0.003         | 0.003         | 0.005      | ✓            |
| fibonacci        | 40    | 0.004         | 0.005         | 0.005      | ✓            |
| fibonacci        | 80    | 0.004         | 0.003         | 0.004      | ✓            |
| fibonacci        | 160   | 0.004         | 0.005         | 0.003      | ✓            |
| factorial        | 3     | 0.005         | 0.011         | 0.004      | ✓            |
| factorial        | 5     | 0.004         | 0.003         | 0.003      | ✓            |
| factorial        | 8     | 0.003         | 0.003         | 0.007      | ✓            |
| factorial        | 13    | 0.004         | 0.003         | 0.003      | ✓            |
| collatz          | 6     | 0.004         | 0.011         | 0.004      | ✓            |
| collatz          | 12    | 0.004         | 0.003         | 0.004      | ✓            |
| collatz          | 27    | 0.003         | 0.003         | 0.003      | ✓            |
| collatz          | 871   | 0.006         | 0.007         | 0.004      | ✓            |
| stress_factorial | 8     | 0.004         | 0.011         | 0.006      | ✓            |
| stress_factorial | 10    | 0.020         | 0.005         | 0.004      | ✓            |
| stress_factorial | 12    | 2.220         | 0.003         | 0.003      | ✓            |
| stress_factorial | 14    | 403.417       | 0.003         | 0.003      | ✓            |

### Architecture Detection

The benchmark script automatically detects your system architecture:
- **RISC-V systems**: Runs executables natively
- **Other architectures**: Uses QEMU user-mode emulation (`qemu-riscv64`)

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
