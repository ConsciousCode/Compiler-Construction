## Project 1: Simple Code Generation
**Team:** Ctrl+Alt+Deletes
#### Members:
- Nicholas Livingstone
- Robert McDaniels
- Key Vollers
### How to run scanner tests...
```bash
cmake . -B build
cd build
cmake --build .
```
#### Run Scanner test 1:
```bash
./test_scanner
```
#### Run Scanner Test 2 (Google Test):
```bash
./scanner_test
```
or
```bash
ctest --output-on-failure
```

## Code Formatting

Before committing, format your code:
```bash
cmake --build build --target format
```
