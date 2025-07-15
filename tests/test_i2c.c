/*
 * I2C Module Unit Tests
 * 
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * 
 * This file is part of Belfhym.
 * 
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "unity.h"
#include "mock_hardware.h"

// Mock the I2C module by redefining its functions
#define blfm_i2c_init mock_i2c_init
#define blfm_i2c_write mock_i2c_write
#define blfm_i2c_read mock_i2c_read

// Include the module types we need
#include "../include/blfm_i2c.h"

void setUp(void) {
    mock_hardware_reset();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_i2c_init(void) {
    // Test I2C initialization
    mock_i2c_init(0); // I2C1
    TEST_ASSERT_TRUE(mock_i2c_devices[0].initialized);
    
    mock_i2c_init(1); // I2C2
    TEST_ASSERT_TRUE(mock_i2c_devices[1].initialized);
}

void test_i2c_write_success(void) {
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = true;
    
    uint8_t test_data[] = {0x12, 0x34, 0x56};
    int result = mock_i2c_write(0, 0x3C, test_data, sizeof(test_data));
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0x3C, mock_i2c_devices[0].device_address);
    TEST_ASSERT_EQUAL(sizeof(test_data), mock_i2c_devices[0].tx_length);
    TEST_ASSERT_EQUAL_MEMORY(test_data, mock_i2c_devices[0].tx_buffer, sizeof(test_data));
}

void test_i2c_write_failure(void) {
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = false;
    
    uint8_t test_data[] = {0x12, 0x34};
    int result = mock_i2c_write(0, 0x3C, test_data, sizeof(test_data));
    
    TEST_ASSERT_EQUAL(-1, result);
}

void test_i2c_read_success(void) {
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = true;
    
    uint8_t expected_data[] = {0xAB, 0xCD, 0xEF};
    mock_i2c_set_device_response(0, 0x68, expected_data, sizeof(expected_data));
    
    uint8_t read_buffer[3];
    int result = mock_i2c_read(0, 0x68, read_buffer, sizeof(read_buffer));
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0x68, mock_i2c_devices[0].device_address);
    TEST_ASSERT_EQUAL_MEMORY(expected_data, read_buffer, sizeof(expected_data));
}

void test_i2c_read_failure(void) {
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = false;
    
    uint8_t read_buffer[3];
    int result = mock_i2c_read(0, 0x68, read_buffer, sizeof(read_buffer));
    
    TEST_ASSERT_EQUAL(-1, result);
}

void test_i2c_invalid_peripheral(void) {
    // Test with invalid peripheral number
    uint8_t test_data[] = {0x12};
    int result = mock_i2c_write(5, 0x3C, test_data, sizeof(test_data));
    
    TEST_ASSERT_EQUAL(-1, result);
}

void test_i2c_null_pointer(void) {
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = true;
    
    // Test with null data pointer
    int result = mock_i2c_write(0, 0x3C, NULL, 1);
    TEST_ASSERT_EQUAL(-1, result);
    
    // Test with null read buffer
    result = mock_i2c_read(0, 0x3C, NULL, 1);
    TEST_ASSERT_EQUAL(-1, result);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_i2c_init);
    RUN_TEST(test_i2c_write_success);
    RUN_TEST(test_i2c_write_failure);
    RUN_TEST(test_i2c_read_success);
    RUN_TEST(test_i2c_read_failure);
    RUN_TEST(test_i2c_invalid_peripheral);
    RUN_TEST(test_i2c_null_pointer);
    
    return UNITY_END();
}