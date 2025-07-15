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

#ifndef UNITY_H
#define UNITY_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Unity test framework macros
#define UNITY_BEGIN() unity_begin()
#define UNITY_END() unity_end()

#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            unity_test_fail(__FILE__, __LINE__, #condition); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            unity_test_fail_with_values(__FILE__, __LINE__, "Expected", #expected, "Actual", #actual, (expected), (actual)); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            unity_test_fail_with_values(__FILE__, __LINE__, "Expected NOT", #expected, "Actual", #actual, (expected), (actual)); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define TEST_ASSERT_NULL(pointer) \
    do { \
        if ((pointer) != NULL) { \
            unity_test_fail(__FILE__, __LINE__, #pointer " should be NULL"); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_NOT_NULL(pointer) \
    do { \
        if ((pointer) == NULL) { \
            unity_test_fail(__FILE__, __LINE__, #pointer " should not be NULL"); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            unity_test_fail_string(__FILE__, __LINE__, "String mismatch", (expected), (actual)); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, length) \
    do { \
        if (memcmp((expected), (actual), (length)) != 0) { \
            unity_test_fail(__FILE__, __LINE__, "Memory content mismatch"); \
            return; \
        } \
        unity_test_pass(); \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running %s...", #test_func); \
        unity_current_test = #test_func; \
        unity_test_count++; \
        test_func(); \
        printf(" PASSED\n"); \
    } while(0)

// Unity framework functions
void unity_begin(void);
int unity_end(void);
void unity_test_pass(void);
void unity_test_fail(const char* file, int line, const char* message);
void unity_test_fail_with_values(const char* file, int line, const char* expected_label, const char* expected_str, const char* actual_label, const char* actual_str, long expected, long actual);
void unity_test_fail_string(const char* file, int line, const char* message, const char* expected, const char* actual);

// Unity framework state
extern const char* unity_current_test;
extern int unity_test_count;
extern int unity_test_failures;

#endif // UNITY_H