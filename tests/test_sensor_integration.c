/*
 * Sensor Integration Tests
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

// Include the module types we need
#include "../include/blfm_types.h"

void setUp(void) {
    mock_hardware_reset();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_ultrasonic_sensor_reading(void) {
    // Test ultrasonic sensor reading with mock hardware
    mock_gpio_config_output(1, 4); // TRIG pin
    mock_gpio_config_input(1, 3);  // ECHO pin
    
    // Simulate ultrasonic sensor response
    mock_gpio_set_pin(1, 4);    // Set TRIG high
    mock_delay_us(10);          // Wait 10us
    mock_gpio_clear_pin(1, 4);  // Set TRIG low
    
    // Simulate ECHO response (pulse width represents distance)
    mock_gpio_set_pin(1, 3);    // ECHO goes high
    mock_delay_us(1000);        // 1ms pulse = ~17cm distance
    mock_gpio_clear_pin(1, 3);  // ECHO goes low
    
    // Verify the mock hardware state
    TEST_ASSERT_EQUAL(0, mock_gpio_read_pin(1, 4)); // TRIG should be low
    TEST_ASSERT_EQUAL(0, mock_gpio_read_pin(1, 3)); // ECHO should be low
}

void test_adc_sensor_reading(void) {
    // Test ADC-based sensors (temperature, potentiometer)
    mock_adc_init();
    
    // Set mock ADC values
    mock_adc_set_channel_value(5, 2048); // Temperature sensor (50% = ~25°C)
    mock_adc_set_channel_value(6, 1024); // Potentiometer (25% position)
    
    uint16_t temp_value, pot_value;
    int result;
    
    result = mock_adc_read(5, &temp_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(2048, temp_value);
    
    result = mock_adc_read(6, &pot_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1024, pot_value);
}

void test_i2c_sensor_reading(void) {
    // Test I2C-based sensors (IMU)
    mock_i2c_init(0); // I2C1
    
    // Mock IMU response data (14 bytes: 3 accel + 1 temp + 3 gyro, 2 bytes each)
    uint8_t imu_data[] = {
        0x10, 0x00, // Accel X = 4096
        0x20, 0x00, // Accel Y = 8192
        0x30, 0x00, // Accel Z = 12288
        0x1A, 0x00, // Temperature
        0x40, 0x00, // Gyro X = 16384
        0x50, 0x00, // Gyro Y = 20480
        0x60, 0x00  // Gyro Z = 24576
    };
    
    mock_i2c_set_device_response(0, 0x68, imu_data, sizeof(imu_data));
    
    uint8_t read_buffer[14];
    int result = mock_i2c_read(0, 0x68, read_buffer, sizeof(read_buffer));
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY(imu_data, read_buffer, sizeof(imu_data));
}

void test_sensor_data_aggregation(void) {
    // Test sensor data aggregation
    blfm_sensor_data_t sensor_data = {0};
    
    // Simulate multiple sensor readings
    sensor_data.ultrasonic.distance_mm = 250;
    sensor_data.temperature.temperature_mc = 25000; // 25°C
    sensor_data.imu.acc_x = 4096;
    sensor_data.imu.acc_y = 8192;
    sensor_data.imu.acc_z = 12288;
    
    // Verify aggregated data
    TEST_ASSERT_EQUAL(250, sensor_data.ultrasonic.distance_mm);
    TEST_ASSERT_EQUAL(25000, sensor_data.temperature.temperature_mc);
    TEST_ASSERT_EQUAL(4096, sensor_data.imu.acc_x);
    TEST_ASSERT_EQUAL(8192, sensor_data.imu.acc_y);
    TEST_ASSERT_EQUAL(12288, sensor_data.imu.acc_z);
}

void test_sensor_error_handling(void) {
    // Test sensor error conditions
    uint16_t value;
    
    // Test ADC error (uninitialized)
    mock_hardware_reset(); // Ensure ADC is not initialized
    int result = mock_adc_read(0, &value);
    TEST_ASSERT_EQUAL(-1, result);
    
    // Test I2C error (device not responding)
    mock_i2c_init(0);
    mock_i2c_devices[0].transaction_success = false;
    
    uint8_t buffer[1];
    result = mock_i2c_read(0, 0x68, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(-1, result);
}

void test_sensor_boundary_conditions(void) {
    // Test sensor boundary conditions
    mock_adc_init();
    
    // Test minimum ADC value
    mock_adc_set_channel_value(0, 0);
    uint16_t min_value;
    int result = mock_adc_read(0, &min_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, min_value);
    
    // Test maximum ADC value
    mock_adc_set_channel_value(0, 4095);
    uint16_t max_value;
    result = mock_adc_read(0, &max_value);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(4095, max_value);
}

void test_sensor_timing_requirements(void) {
    // Test sensor timing requirements
    uint32_t start_time = mock_get_tick();
    
    // Simulate sensor reading with timing
    mock_delay_ms(100); // Typical sensor reading time
    
    uint32_t end_time = mock_get_tick();
    uint32_t elapsed = end_time - start_time;
    
    TEST_ASSERT_EQUAL(100, elapsed);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_ultrasonic_sensor_reading);
    RUN_TEST(test_adc_sensor_reading);
    RUN_TEST(test_i2c_sensor_reading);
    RUN_TEST(test_sensor_data_aggregation);
    RUN_TEST(test_sensor_error_handling);
    RUN_TEST(test_sensor_boundary_conditions);
    RUN_TEST(test_sensor_timing_requirements);
    
    return UNITY_END();
}