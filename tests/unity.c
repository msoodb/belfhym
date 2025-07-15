/*
 * Unity Test Framework - Simplified version for Belfhym
 * Based on Unity by ThrowTheSwitch.org
 * 
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * 
 * This file is part of Belfhym.
 * 
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "unity.h"

// Unity framework state
const char* unity_current_test = NULL;
int unity_test_count = 0;
int unity_test_failures = 0;

void unity_begin(void) {
    unity_test_count = 0;
    unity_test_failures = 0;
    printf("Unity Test Framework - Belfhym Edition\n");
    printf("======================================\n");
}

int unity_end(void) {
    printf("\n======================================\n");
    printf("Tests run: %d\n", unity_test_count);
    printf("Failures: %d\n", unity_test_failures);
    printf("Success rate: %.1f%%\n", 
           unity_test_count > 0 ? ((float)(unity_test_count - unity_test_failures) / unity_test_count) * 100.0 : 0.0);
    
    if (unity_test_failures == 0) {
        printf("All tests PASSED!\n");
    } else {
        printf("Some tests FAILED!\n");
    }
    
    return unity_test_failures;
}

void unity_test_pass(void) {
    // Test passed - no action needed
}

void unity_test_fail(const char* file, int line, const char* message) {
    unity_test_failures++;
    printf(" FAILED\n");
    printf("  File: %s\n", file);
    printf("  Line: %d\n", line);
    printf("  Message: %s\n", message);
    printf("\n");
}

void unity_test_fail_with_values(const char* file, int line, const char* expected_label, const char* expected_str, const char* actual_label, const char* actual_str, long expected, long actual) {
    unity_test_failures++;
    printf(" FAILED\n");
    printf("  File: %s\n", file);
    printf("  Line: %d\n", line);
    printf("  %s: %s (%ld)\n", expected_label, expected_str, expected);
    printf("  %s: %s (%ld)\n", actual_label, actual_str, actual);
    printf("\n");
}

void unity_test_fail_string(const char* file, int line, const char* message, const char* expected, const char* actual) {
    unity_test_failures++;
    printf(" FAILED\n");
    printf("  File: %s\n", file);
    printf("  Line: %d\n", line);
    printf("  Message: %s\n", message);
    printf("  Expected: \"%s\"\n", expected);
    printf("  Actual: \"%s\"\n", actual);
    printf("\n");
}