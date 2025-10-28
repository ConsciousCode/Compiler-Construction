## Project 1: Simple Code Generation
**Team:** Ctrl+Alt+Deletes
#### Members:
- Nicholas Livingstone
- Robert McDaniels
- Key Vollers
### How to build the compiler
```bash
cmake -S . -B build
cmake --build build -j
```
### Run Scanner test 1:
```bash
./test_scanner
```
### Run Scanner Test 2 (Google Test):
```bash
./scanner_test
```
or
```bash
ctest --output-on-failure
```
### How to Compile `.while` file
- Build the Compiler
- Run the following code inserting the `.while` file that you whish to compile
```bash
./build/wisc ./examples/fibonacci.while
```
### How to Run Code on RISC-V machine
#### Fibonacci
Output vars order, `a, b, n, output, t, z`
```bash
gcc fibonacci.s ./test/main.c -DPROGRAM_NAME=finonacci -o run_fib
./run_fib 0 1 10 0 0 0
```
#### Factorial
Output vars order, `output, x, y, z`
```bash
gcc factorial.s ./test/main.c -DPROGRAM_NAME=factorial -o run_fact
./run_fact 0 5 0 0 
```
#### Collatz
Output vars order, `a, b, n, output, t, z`
```bash
gcc collatz.s ./test/main.c -DPROGRAM_NAME=collatz -o run_clz
./run_clz 10 0 0 0 0 0 
```
