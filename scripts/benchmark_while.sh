#!/bin/bash

# Script to run and benchmark WHILE programs
# Usage: ./benchmark_while.sh [build_dir]

BUILD_DIR="${1:-build}"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect architecture
ARCH=$(uname -m)
if [ "$ARCH" == "riscv64" ]; then
    echo "Detected native RISC-V architecture"
    RISC_RUNNER=""
else
    echo "Detected non-RISC-V architecture ($ARCH), using QEMU"
    RISC_RUNNER="qemu-riscv64"
    # Check if qemu is available
    if ! command -v qemu-riscv64 &> /dev/null; then
        echo -e "${RED}Error: qemu-riscv64 not found. Please install it.${NC}"
        exit 1
    fi
fi

echo ""
echo "================================================================================"
echo "                        WHILE Program Benchmark Suite"
echo "================================================================================"
echo ""

# Function to run a single test
# Usage: run_test <program_name> <display_input> <args...>
run_test() {
    local program_name=$1
    local display_input=$2
    shift 2
    local args="$@"
    
    local risc_exec="${BUILD_DIR}/${program_name}_risc"
    local c_exec="${BUILD_DIR}/${program_name}_c"
    
    # Check if executables exist
    if [ ! -f "$risc_exec" ] || [ ! -f "$c_exec" ]; then
        printf "%-20s | %-10s | %-12s | %-12s | %s\n" \
            "$program_name" "$display_input" "N/A" "N/A" "✗ Missing"
        return 1
    fi
    
    # Run RISC-V version with timing
    risc_time=$( { time $RISC_RUNNER "$risc_exec" $args > /tmp/risc_output 2>&1; } 2>&1 | grep real | awk '{print $2}')
    risc_output=$(cat /tmp/risc_output)
    
    # Run C version with timing
    c_time=$( { time "$c_exec" $args > /tmp/c_output 2>&1; } 2>&1 | grep real | awk '{print $2}')
    c_output=$(cat /tmp/c_output)
    
    # Compare outputs and display results
    if [ "$risc_output" == "$c_output" ]; then
        status="${GREEN}✓${NC}"
    else
        status="${RED}✗${NC}"
    fi
    
    printf "%-20s | %-10s | ${YELLOW}%-12s${NC} | ${YELLOW}%-12s${NC} | %b\n" \
        "$program_name" "$display_input" "$risc_time" "$c_time" "$status"
}

# Print table header
printf "%-20s | %-10s | %-12s | %-12s | %s\n" "Program" "Input" "RISC-V Time" "C Time" "Output Match"
printf "%.s-" {1..75}
echo ""

# Fibonacci tests (Variables: a, b, n, output, t, z) - n is the input
run_test fibonacci 10  0 1 10 0 0 0
run_test fibonacci 40  0 1 40 0 0 0
run_test fibonacci 80  0 1 80 0 0 0
run_test fibonacci 160 0 1 160 0 0 0

# Factorial tests (Variables: output, x, y, z) - x is the input
run_test factorial 3  0 3 0 0
run_test factorial 5  0 5 0 0
run_test factorial 8  0 8 0 0
run_test factorial 13 0 13 0 0

# Collatz tests (Variables: a, b, n, output, t, z) - a is the input
run_test collatz 6   6 0 0 0 0 0
run_test collatz 12  12 0 0 0 0 0
run_test collatz 27  27 0 0 0 0 0
run_test collatz 871 871 0 0 0 0 0

# Stress factorial tests (Variables: input, i, output, t, x, y, z) - first arg is the input
run_test stress_factorial 8  8 0 0 0 0 0 0
run_test stress_factorial 10 10 0 0 0 0 0 0
run_test stress_factorial 12 12 0 0 0 0 0 0
run_test stress_factorial 14 14 0 0 0 0 0 0

printf "%.s-" {1..75}
echo ""
echo "Benchmark Complete"
echo ""

# Cleanup
rm -f /tmp/risc_output /tmp/c_output

