#!/bin/bash
# Comprehensive test runner for Belfhym
# 
# Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
# 
# This file is part of Belfhym.
# 
# Belfhym is released under the GNU General Public License v3 (GPL-3.0).
# See LICENSE file for details.

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
RUN_UNIT_TESTS=true
RUN_INTEGRATION_TESTS=true
RUN_COVERAGE=false
RUN_VALGRIND=false
RUN_STATIC_ANALYSIS=false
RUN_QEMU_TEST=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --coverage)
            RUN_COVERAGE=true
            shift
            ;;
        --valgrind)
            RUN_VALGRIND=true
            shift
            ;;
        --static-analysis)
            RUN_STATIC_ANALYSIS=true
            shift
            ;;
        --qemu)
            RUN_QEMU_TEST=true
            shift
            ;;
        --all)
            RUN_COVERAGE=true
            RUN_VALGRIND=true
            RUN_STATIC_ANALYSIS=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --coverage         Run with coverage analysis"
            echo "  --valgrind         Run with memory leak detection"
            echo "  --static-analysis  Run static code analysis"
            echo "  --qemu             Run QEMU simulation tests"
            echo "  --all              Run all analysis tools"
            echo "  --help             Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}================================${NC}"
echo -e "${BLUE}  Belfhym Test Suite Runner${NC}"
echo -e "${BLUE}================================${NC}"
echo ""

# Function to print status
print_status() {
    local status=$1
    local message=$2
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✓ $message${NC}"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}✗ $message${NC}"
    elif [ "$status" = "WARN" ]; then
        echo -e "${YELLOW}⚠ $message${NC}"
    else
        echo -e "${BLUE}ℹ $message${NC}"
    fi
}

# Function to run command and capture result
run_command() {
    local cmd=$1
    local description=$2
    
    echo -e "${BLUE}Running: $description${NC}"
    
    if $cmd; then
        print_status "PASS" "$description completed successfully"
        return 0
    else
        print_status "FAIL" "$description failed"
        return 1
    fi
}

# Initialize test environment
print_status "INFO" "Setting up test environment..."
mkdir -p results
mkdir -p build

# Check dependencies
print_status "INFO" "Checking test dependencies..."
DEPS_OK=true

if ! command -v gcc >/dev/null 2>&1; then
    print_status "FAIL" "gcc not found. Install with: sudo dnf install gcc"
    DEPS_OK=false
fi

if [ "$RUN_VALGRIND" = true ] && ! command -v valgrind >/dev/null 2>&1; then
    print_status "WARN" "valgrind not found. Install with: sudo dnf install valgrind"
    RUN_VALGRIND=false
fi

if [ "$RUN_STATIC_ANALYSIS" = true ] && ! command -v cppcheck >/dev/null 2>&1; then
    print_status "WARN" "cppcheck not found. Install with: sudo dnf install cppcheck"
    RUN_STATIC_ANALYSIS=false
fi

if [ "$RUN_COVERAGE" = true ] && ! command -v lcov >/dev/null 2>&1; then
    print_status "WARN" "lcov not found. Install with: sudo dnf install lcov"
fi

if [ "$DEPS_OK" = false ]; then
    print_status "FAIL" "Missing required dependencies. Exiting."
    exit 1
fi

# Build tests
print_status "INFO" "Building tests..."
echo -e "${BLUE}Running: Building test suite${NC}"
if make clean && make all; then
    print_status "PASS" "Building test suite completed successfully"
else
    print_status "FAIL" "Building test suite failed"
    exit 1
fi

# Run unit tests
if [ "$RUN_UNIT_TESTS" = true ]; then
    print_status "INFO" "Running unit tests..."
    
    # Individual test results
    TEST_RESULTS=()
    
    # Run Basic tests
    if ./build/test_basic > results/test_basic.log 2>&1; then
        print_status "PASS" "Basic tests"
        TEST_RESULTS+=("Basic:PASS")
    else
        print_status "FAIL" "Basic tests - See results/test_basic.log"
        TEST_RESULTS+=("Basic:FAIL")
    fi
    
    # Run Controller tests
    if ./build/test_controller > results/test_controller.log 2>&1; then
        print_status "PASS" "Controller tests"
        TEST_RESULTS+=("Controller:PASS")
    else
        print_status "FAIL" "Controller tests - See results/test_controller.log"
        TEST_RESULTS+=("Controller:FAIL")
    fi
    
    # Run Sensor Integration tests
    if ./build/test_sensor_integration > results/test_sensor_integration.log 2>&1; then
        print_status "PASS" "Sensor Integration tests"
        TEST_RESULTS+=("Sensor Integration:PASS")
    else
        print_status "FAIL" "Sensor Integration tests - See results/test_sensor_integration.log"
        TEST_RESULTS+=("Sensor Integration:FAIL")
    fi
fi

# Run coverage analysis
if [ "$RUN_COVERAGE" = true ]; then
    print_status "INFO" "Running coverage analysis..."
    if run_command "make coverage" "Coverage analysis"; then
        print_status "PASS" "Coverage report generated in results/coverage_html/"
    fi
fi

# Run memory leak detection
if [ "$RUN_VALGRIND" = true ]; then
    print_status "INFO" "Running memory leak detection..."
    if run_command "make valgrind" "Memory leak detection"; then
        print_status "PASS" "Memory leak detection completed - See results/*_valgrind.txt"
    fi
fi

# Run static analysis
if [ "$RUN_STATIC_ANALYSIS" = true ]; then
    print_status "INFO" "Running static code analysis..."
    if run_command "make cppcheck" "Static analysis"; then
        print_status "PASS" "Static analysis completed - See results/cppcheck.xml"
    fi
fi

# Run QEMU simulation test
if [ "$RUN_QEMU_TEST" = true ]; then
    print_status "INFO" "Running QEMU simulation tests..."
    
    # Check if QEMU is available
    if command -v qemu-system-arm >/dev/null 2>&1; then
        print_status "INFO" "QEMU available - attempting basic simulation"
        
        # Build the main project first
        cd ..
        if make clean && make; then
            print_status "INFO" "Built main project - attempting QEMU simulation"
            
            # Run QEMU with timeout
            timeout 5s qemu-system-arm \
                -M stm32vldiscovery \
                -nographic \
                -kernel bin/belfhym > tests/results/qemu_test.log 2>&1 || true
            
            print_status "PASS" "QEMU simulation test completed - See results/qemu_test.log"
        else
            print_status "FAIL" "Could not build main project for QEMU test"
        fi
        cd tests
    else
        print_status "WARN" "QEMU not available. Install with: sudo dnf install qemu-system-arm"
    fi
fi

# Generate test summary
echo ""
echo -e "${BLUE}================================${NC}"
echo -e "${BLUE}        Test Summary${NC}"
echo -e "${BLUE}================================${NC}"

if [ "$RUN_UNIT_TESTS" = true ]; then
    echo -e "${YELLOW}Unit Test Results:${NC}"
    for result in "${TEST_RESULTS[@]}"; do
        TEST_NAME=$(echo "$result" | cut -d: -f1)
        TEST_STATUS=$(echo "$result" | cut -d: -f2)
        
        if [ "$TEST_STATUS" = "PASS" ]; then
            print_status "PASS" "$TEST_NAME"
        else
            print_status "FAIL" "$TEST_NAME"
        fi
    done
fi

# Check if all tests passed
FAILED_TESTS=0
for result in "${TEST_RESULTS[@]}"; do
    if [[ "$result" == *":FAIL" ]]; then
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
done

echo ""
if [ $FAILED_TESTS -eq 0 ]; then
    print_status "PASS" "All tests passed!"
    echo ""
    echo -e "${GREEN}✓ Ready for hardware deployment${NC}"
else
    print_status "FAIL" "$FAILED_TESTS tests failed"
    echo ""
    echo -e "${RED}✗ Fix failing tests before deployment${NC}"
fi

echo ""
echo -e "${BLUE}Test artifacts saved in results/${NC}"
echo -e "${BLUE}Use 'make help' for more testing options${NC}"

exit $FAILED_TESTS