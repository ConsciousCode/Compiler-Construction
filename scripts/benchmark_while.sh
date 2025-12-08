#!/bin/bash

# Script to run and benchmark WHILE programs
# Usage: ./benchmark_while.sh [build_dir]

BUILD_DIR="${1:-build}"
MISMATCH_FILE="benchmark_mismatches.txt"

# Column width for the Program name column. If a program name exceeds
# this, we'll truncate and append an ellipsis.
PROG_COL=20

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

# Initialize mismatch file
> "$MISMATCH_FILE"
echo "WHILE Program Benchmark - Output Mismatches" > "$MISMATCH_FILE"
echo "Generated: $(date)" >> "$MISMATCH_FILE"
echo "================================================================================" >> "$MISMATCH_FILE"
echo "" >> "$MISMATCH_FILE"

# Function to run a single test
# Usage: run_test <program_name> <display_input> <args...>
run_test() {
    local program_name=$1
    local display_input=$2
    shift 2
    local args="$@"
    
    # Detect architecture (matching CMake logic)
    local ARCH=$(uname -m)
    case "$ARCH" in
        x86_64|amd64) NATIVE_ARCH="x86_64" ;;
        aarch64|arm64) NATIVE_ARCH="aarch64" ;;
        riscv64) NATIVE_ARCH="riscv64" ;;
        *) NATIVE_ARCH="$ARCH" ;;
    esac
    
    local risc_exec="${BUILD_DIR}/while_progs/bin/riscv64/${program_name}_risc"
    local c_exec="${BUILD_DIR}/while_progs/bin/${NATIVE_ARCH}/${program_name}_c_cfg"
    
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
    	status="✓"
    else
    	status="✗"

        # Write mismatch to file instead of stdout
    	echo "------------------------------------------------------------" >> "$MISMATCH_FILE"
    	echo " MISMATCH FOR: $program_name  input=($args)" >> "$MISMATCH_FILE"
    	echo "------------------------------------------------------------" >> "$MISMATCH_FILE"
    	echo "Expected output (C version):" >> "$MISMATCH_FILE"
    	echo "----------------------------" >> "$MISMATCH_FILE"
    	echo "$c_output" >> "$MISMATCH_FILE"
    	echo "" >> "$MISMATCH_FILE"
    	echo "Actual output (RISC-V version):" >> "$MISMATCH_FILE"
    	echo "-------------------------------" >> "$MISMATCH_FILE"
    	echo "$risc_output" >> "$MISMATCH_FILE"
    	echo "" >> "$MISMATCH_FILE"
    fi

    # Truncate program name if it's longer than PROG_COL and append '...'
    local display_name="$program_name"
    if [ ${#display_name} -gt "$PROG_COL" ]; then
        display_name="${display_name:0:$((PROG_COL-3))}..."
    fi

    printf "%-${PROG_COL}s | %-10s | ${YELLOW}%-12s${NC} | ${YELLOW}%-12s${NC} | %b\n" \
        "$display_name" "$display_input" "$risc_time" "$c_time" "$status"
}

# Print table header
printf "%-${PROG_COL}s | %-10s | %-12s | %-12s | %s\n" "Program" "Input" "RISC-V Time" "C Time" "Output Match"
printf "%.s-" {1..75}
echo ""

# Fibonacci tests - input: n
run_test fibonacci 10  10
run_test fibonacci 40  40
run_test fibonacci 80  80
run_test fibonacci 160 160

# Factorial tests - input: x
run_test factorial 3  3
run_test factorial 5  5
run_test factorial 8  8
run_test factorial 13 13

# Collatz tests - input: input
run_test collatz 6   6
run_test collatz 12  12
run_test collatz 27  27
run_test collatz 871 871

# Stress factorial tests - input: input
run_test stress_factorial 8  8
run_test stress_factorial 10 10
run_test stress_factorial 12 12

# add - no live inputs
run_test add

# closest_prime - input: input
run_test closest_prime 5 5

# comparatornetwork_bestknown_for_11 - inputs: w0..w10
run_test comparatornetwork_bestknown_for_11 desc 11 10 9 8 7 6 5 4 3 2 1

# countTriangularNumbers - input: goUpToNumber
run_test countTriangularNumbers 100 100

# loop - input: n
run_test loop 5 5

# madprimes - input: W
run_test madprimes 50 50

# primescounter - input: range
run_test primescounter 100 100

# stress_gcd_mod and stress_gcd_sub - inputs: input, input2
run_test stress_gcd_mod 1234567 1234567 7654321
run_test stress_gcd_sub 1234567 1234567 7654321

# stress_prime_count - input: n
run_test stress_prime_count 50 50

# sumfactors - input: N
run_test sumfactors 20 20

printf "%.s-" {1..75}
echo ""
echo "Benchmark Complete"
echo ""

# Report mismatch file location if there were any mismatches
if [ -s "$MISMATCH_FILE" ] && [ $(wc -l < "$MISMATCH_FILE") -gt 4 ]; then
    echo -e "${RED}Mismatches detected! Details written to: $MISMATCH_FILE${NC}"
else
    echo -e "${GREEN}All tests passed!${NC}"
    rm -f "$MISMATCH_FILE"
fi

# Cleanup
rm -f /tmp/risc_output /tmp/c_output

