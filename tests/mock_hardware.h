/*
 * Mock Hardware Abstraction Layer for Belfhym Testing
 * 
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * 
 * This file is part of Belfhym.
 * 
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Mock GPIO
typedef struct {
    uint32_t pin_states[16];    // 16 pins per port
    uint32_t pin_configs[16];   // Pin configurations
} mock_gpio_port_t;

extern mock_gpio_port_t mock_gpio_ports[3];  // A, B, C ports

// Mock ADC
typedef struct {
    uint16_t channel_values[16];  // ADC channel values
    bool initialized;
} mock_adc_t;

extern mock_adc_t mock_adc;

// Mock I2C
typedef struct {
    uint8_t tx_buffer[256];
    uint8_t rx_buffer[256];
    size_t tx_length;
    size_t rx_length;
    uint8_t device_address;
    bool initialized;
    bool transaction_success;
} mock_i2c_t;

extern mock_i2c_t mock_i2c_devices[2];  // I2C1 and I2C2

// Mock UART
typedef struct {
    uint8_t tx_buffer[1024];
    uint8_t rx_buffer[1024];
    size_t tx_length;
    size_t rx_length;
    bool initialized;
} mock_uart_t;

extern mock_uart_t mock_uart;

// Mock PWM
typedef struct {
    uint32_t duty_cycle;
    uint32_t frequency;
    bool enabled;
} mock_pwm_channel_t;

extern mock_pwm_channel_t mock_pwm_channels[4];

// Mock system time
extern uint32_t mock_system_ticks;

// Mock hardware initialization
void mock_hardware_init(void);
void mock_hardware_reset(void);

// Mock GPIO functions
void mock_gpio_set_pin(uint32_t port, uint32_t pin);
void mock_gpio_clear_pin(uint32_t port, uint32_t pin);
uint32_t mock_gpio_read_pin(uint32_t port, uint32_t pin);
void mock_gpio_config_output(uint32_t port, uint32_t pin);
void mock_gpio_config_input(uint32_t port, uint32_t pin);

// Mock ADC functions
void mock_adc_init(void);
int mock_adc_read(uint8_t channel, uint16_t *value);
void mock_adc_set_channel_value(uint8_t channel, uint16_t value);

// Mock I2C functions
void mock_i2c_init(uint8_t peripheral);
int mock_i2c_write(uint8_t peripheral, uint8_t addr, const uint8_t *data, size_t len);
int mock_i2c_read(uint8_t peripheral, uint8_t addr, uint8_t *data, size_t len);
void mock_i2c_set_device_response(uint8_t peripheral, uint8_t addr, const uint8_t *response, size_t len);

// Mock UART functions
void mock_uart_init(void);
void mock_uart_transmit(const uint8_t *data, size_t len);
size_t mock_uart_receive(uint8_t *data, size_t max_len);
void mock_uart_inject_data(const uint8_t *data, size_t len);

// Mock PWM functions
void mock_pwm_init(void);
void mock_pwm_set_duty_cycle(uint8_t channel, uint32_t duty);
void mock_pwm_set_frequency(uint8_t channel, uint32_t frequency);
void mock_pwm_enable(uint8_t channel);
void mock_pwm_disable(uint8_t channel);

// Mock delay functions
void mock_delay_ms(uint32_t ms);
void mock_delay_us(uint32_t us);

// Mock system time
uint32_t mock_get_tick(void);
void mock_advance_time(uint32_t ms);

// Test utilities
void mock_print_gpio_state(void);
void mock_print_i2c_transactions(void);
void mock_print_uart_data(void);

#endif // MOCK_HARDWARE_H