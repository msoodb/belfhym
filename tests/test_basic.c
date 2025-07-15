/*
 * Basic Unit Tests for Belfhym Testing Framework
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

void setUp(void) {
    mock_hardware_reset();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_unity_framework(void) {
    // Test the Unity framework itself
    TEST_ASSERT_TRUE(true);
    TEST_ASSERT_FALSE(false);
    TEST_ASSERT_EQUAL(42, 42);
    TEST_ASSERT_NOT_EQUAL(42, 24);
}

void test_mock_hardware_init(void) {
    // Test mock hardware initialization
    mock_hardware_init();
    
    // Verify initial state
    TEST_ASSERT_EQUAL(0, mock_system_ticks);
    TEST_ASSERT_FALSE(mock_adc.initialized);
    TEST_ASSERT_FALSE(mock_uart.initialized);
}

void test_mock_gpio_basic(void) {
    // Test basic GPIO operations
    mock_gpio_config_output(0, 5);  // Port A, Pin 5
    mock_gpio_set_pin(0, 5);
    TEST_ASSERT_EQUAL(1, mock_gpio_read_pin(0, 5));
    
    mock_gpio_clear_pin(0, 5);
    TEST_ASSERT_EQUAL(0, mock_gpio_read_pin(0, 5));
}

void test_mock_adc_basic(void) {
    // Test basic ADC operations
    mock_adc_init();
    TEST_ASSERT_TRUE(mock_adc.initialized);
    
    mock_adc_set_channel_value(0, 1234);
    
    uint16_t value;
    int result = mock_adc_read(0, &value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1234, value);
}

void test_mock_i2c_basic(void) {
    // Test basic I2C operations
    mock_i2c_init(0);
    TEST_ASSERT_TRUE(mock_i2c_devices[0].initialized);
    
    // Set up successful transaction
    mock_i2c_devices[0].transaction_success = true;
    
    uint8_t test_data[] = {0x12, 0x34, 0x56};
    int result = mock_i2c_write(0, 0x3C, test_data, sizeof(test_data));
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0x3C, mock_i2c_devices[0].device_address);
    TEST_ASSERT_EQUAL(sizeof(test_data), mock_i2c_devices[0].tx_length);
}

void test_mock_uart_basic(void) {
    // Test basic UART operations
    mock_uart_init();
    TEST_ASSERT_TRUE(mock_uart.initialized);
    
    uint8_t test_data[] = "Hello, World!";
    mock_uart_transmit(test_data, sizeof(test_data) - 1);
    
    TEST_ASSERT_EQUAL(sizeof(test_data) - 1, mock_uart.tx_length);
    TEST_ASSERT_EQUAL_MEMORY(test_data, mock_uart.tx_buffer, sizeof(test_data) - 1);
}

void test_mock_timing(void) {
    // Test timing functions
    uint32_t start_time = mock_get_tick();
    TEST_ASSERT_EQUAL(0, start_time);
    
    mock_delay_ms(100);
    uint32_t end_time = mock_get_tick();
    TEST_ASSERT_EQUAL(100, end_time);
    
    mock_advance_time(50);
    TEST_ASSERT_EQUAL(150, mock_get_tick());
}

void test_data_structures(void) {
    // Test basic data structure operations
    typedef struct {
        uint16_t value;
        bool valid;
    } test_data_t;
    
    test_data_t data1 = {1234, true};
    test_data_t data2 = {1234, true};
    test_data_t data3 = {5678, false};
    
    // Test individual fields instead of memory compare due to padding
    TEST_ASSERT_EQUAL(data1.value, data2.value);
    TEST_ASSERT_EQUAL(data1.valid, data2.valid);
    TEST_ASSERT_NOT_EQUAL(data1.value, data3.value);
    TEST_ASSERT_NOT_EQUAL(data1.valid, data3.valid);
}

void test_boundary_conditions(void) {
    // Test boundary conditions
    mock_adc_init();
    
    // Test minimum value
    mock_adc_set_channel_value(0, 0);
    uint16_t min_value;
    int result = mock_adc_read(0, &min_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, min_value);
    
    // Test maximum value
    mock_adc_set_channel_value(0, 4095);
    uint16_t max_value;
    result = mock_adc_read(0, &max_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(4095, max_value);
}

void test_error_conditions(void) {
    // Test error conditions
    uint16_t value;
    
    // Test ADC read without initialization
    mock_hardware_reset(); // Ensure ADC is not initialized
    int result = mock_adc_read(0, &value);
    TEST_ASSERT_EQUAL(-1, result);
    
    // Test ADC read with null pointer
    mock_adc_init();
    result = mock_adc_read(0, NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    // Test I2C with invalid peripheral
    uint8_t data[] = {0x12};
    result = mock_i2c_write(5, 0x3C, data, sizeof(data));
    TEST_ASSERT_EQUAL(-1, result);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_unity_framework);
    RUN_TEST(test_mock_hardware_init);
    RUN_TEST(test_mock_gpio_basic);
    RUN_TEST(test_mock_adc_basic);
    RUN_TEST(test_mock_i2c_basic);
    RUN_TEST(test_mock_uart_basic);
    RUN_TEST(test_mock_timing);
    RUN_TEST(test_data_structures);
    RUN_TEST(test_boundary_conditions);
    RUN_TEST(test_error_conditions);
    
    UNITY_END();
    return 0;
}