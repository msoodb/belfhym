# Makefile for Belfhym Unit Tests
# 
# Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
# 
# This file is part of Belfhym.
# 
# Belfhym is released under the GNU General Public License v3 (GPL-3.0).
# See LICENSE file for details.

# Host compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O0
CPPFLAGS = -I../include -I.
LDFLAGS = 

# Test directories
TEST_DIR = .
BUILD_DIR = build
RESULTS_DIR = results

# Source files
UNITY_SRCS = unity.c mock_hardware.c
TEST_SRCS = test_basic.c test_controller.c test_sensor_integration.c

# Object files
UNITY_OBJS = $(UNITY_SRCS:%.c=$(BUILD_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:%.c=$(BUILD_DIR)/%.o)

# Test executables
TEST_EXECUTABLES = $(TEST_SRCS:%.c=$(BUILD_DIR)/%)

# Default target
all: setup $(TEST_EXECUTABLES)

# Setup directories
setup:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(RESULTS_DIR)

# Unity framework
$(BUILD_DIR)/unity.o: unity.c unity.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/mock_hardware.o: mock_hardware.c mock_hardware.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Test executables
$(BUILD_DIR)/test_basic: $(BUILD_DIR)/test_basic.o $(UNITY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_controller: $(BUILD_DIR)/test_controller.o $(UNITY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_sensor_integration: $(BUILD_DIR)/test_sensor_integration.o $(UNITY_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Generic test object compilation
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Run all tests
test: $(TEST_EXECUTABLES)
	@echo "Running all tests..."
	@for test in $(TEST_EXECUTABLES); do \
		echo ""; \
		echo "=== Running $$test ==="; \
		./$$test > $(RESULTS_DIR)/$$(basename $$test).txt 2>&1; \
		if [ $$? -eq 0 ]; then \
			echo "✓ $$test PASSED"; \
		else \
			echo "✗ $$test FAILED"; \
			cat $(RESULTS_DIR)/$$(basename $$test).txt; \
		fi; \
	done
	@echo ""
	@echo "Test results saved in $(RESULTS_DIR)/"

# Run individual tests
test-basic: $(BUILD_DIR)/test_basic
	@echo "Running Basic tests..."
	@./$(BUILD_DIR)/test_basic

test-controller: $(BUILD_DIR)/test_controller
	@echo "Running Controller tests..."
	@./$(BUILD_DIR)/test_controller

test-sensor-integration: $(BUILD_DIR)/test_sensor_integration
	@echo "Running Sensor Integration tests..."
	@./$(BUILD_DIR)/test_sensor_integration

# Run tests with coverage (if gcov is available)
coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: clean all test
	@echo "Generating coverage report..."
	@gcov -r $(BUILD_DIR)/*.gcda
	@if command -v lcov >/dev/null 2>&1; then \
		lcov --capture --directory $(BUILD_DIR) --output-file $(RESULTS_DIR)/coverage.info; \
		genhtml $(RESULTS_DIR)/coverage.info --output-directory $(RESULTS_DIR)/coverage_html; \
		echo "Coverage report generated in $(RESULTS_DIR)/coverage_html/"; \
	else \
		echo "Install lcov for HTML coverage reports: sudo dnf install lcov"; \
	fi

# Memory leak detection with valgrind
valgrind: $(TEST_EXECUTABLES)
	@echo "Running tests with valgrind..."
	@if command -v valgrind >/dev/null 2>&1; then \
		for test in $(TEST_EXECUTABLES); do \
			echo ""; \
			echo "=== Memory check: $$test ==="; \
			valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
				./$$test > $(RESULTS_DIR)/$$(basename $$test)_valgrind.txt 2>&1; \
		done; \
	else \
		echo "Install valgrind for memory leak detection: sudo dnf install valgrind"; \
	fi

# Static analysis with cppcheck
cppcheck:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --inconclusive --std=c99 \
			--suppress=missingIncludeSystem \
			--xml --xml-version=2 \
			$(TEST_SRCS) $(UNITY_SRCS) > $(RESULTS_DIR)/cppcheck.xml 2>&1; \
		echo "Static analysis results saved in $(RESULTS_DIR)/cppcheck.xml"; \
	else \
		echo "Install cppcheck for static analysis: sudo dnf install cppcheck"; \
	fi

# Clean build artifacts
clean:
	@echo "Cleaning test artifacts..."
	@rm -rf $(BUILD_DIR) $(RESULTS_DIR)
	@rm -f *.gcov *.gcda *.gcno

# Install test dependencies (Fedora)
install-deps:
	@echo "Installing test dependencies..."
	@sudo dnf install -y gcc valgrind cppcheck lcov

# Show help
help:
	@echo "Belfhym Test Framework"
	@echo "====================="
	@echo ""
	@echo "Available targets:"
	@echo "  all                - Build all tests"
	@echo "  test               - Run all tests"
	@echo "  test-basic         - Run Basic tests only"
	@echo "  test-controller    - Run Controller tests only"
	@echo "  test-sensor-integration - Run Sensor Integration tests only"
	@echo "  coverage           - Run tests with coverage analysis"
	@echo "  valgrind           - Run tests with memory leak detection"
	@echo "  cppcheck           - Run static analysis"
	@echo "  clean              - Clean build artifacts"
	@echo "  install-deps       - Install test dependencies (Fedora)"
	@echo "  help               - Show this help message"

# QEMU simulation testing
qemu-test:
	@echo "QEMU simulation testing is not yet implemented"
	@echo "For now, use the unit tests above"
	@echo "Future: Will simulate STM32F103 behavior"

.PHONY: all setup test test-i2c test-controller test-sensor-integration coverage valgrind cppcheck clean install-deps help qemu-test