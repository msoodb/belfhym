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

#include "mock_hardware.h"
#include <stdio.h>

// Mock hardware state
mock_gpio_port_t mock_gpio_ports[3];
mock_adc_t mock_adc;
mock_i2c_t mock_i2c_devices[2];
mock_uart_t mock_uart;
mock_pwm_channel_t mock_pwm_channels[4];
uint32_t mock_system_ticks = 0;

void mock_hardware_init(void) {
    mock_hardware_reset();
}

void mock_hardware_reset(void) {
    // Reset GPIO
    memset(mock_gpio_ports, 0, sizeof(mock_gpio_ports));
    
    // Reset ADC
    memset(&mock_adc, 0, sizeof(mock_adc));
    
    // Reset I2C
    memset(mock_i2c_devices, 0, sizeof(mock_i2c_devices));
    
    // Reset UART
    memset(&mock_uart, 0, sizeof(mock_uart));
    
    // Reset PWM
    memset(mock_pwm_channels, 0, sizeof(mock_pwm_channels));
    
    // Reset system time
    mock_system_ticks = 0;
}

// Mock GPIO functions
void mock_gpio_set_pin(uint32_t port, uint32_t pin) {
    if (port < 3 && pin < 16) {
        mock_gpio_ports[port].pin_states[pin] = 1;
    }
}

void mock_gpio_clear_pin(uint32_t port, uint32_t pin) {
    if (port < 3 && pin < 16) {
        mock_gpio_ports[port].pin_states[pin] = 0;
    }
}

uint32_t mock_gpio_read_pin(uint32_t port, uint32_t pin) {
    if (port < 3 && pin < 16) {
        return mock_gpio_ports[port].pin_states[pin];
    }
    return 0;
}

void mock_gpio_config_output(uint32_t port, uint32_t pin) {
    if (port < 3 && pin < 16) {
        mock_gpio_ports[port].pin_configs[pin] = 1; // 1 = output
    }
}

void mock_gpio_config_input(uint32_t port, uint32_t pin) {
    if (port < 3 && pin < 16) {
        mock_gpio_ports[port].pin_configs[pin] = 0; // 0 = input
    }
}

// Mock ADC functions
void mock_adc_init(void) {
    mock_adc.initialized = true;
}

int mock_adc_read(uint8_t channel, uint16_t *value) {
    if (!mock_adc.initialized || channel >= 16 || !value) {
        return -1;
    }
    
    *value = mock_adc.channel_values[channel];
    return 0;
}

void mock_adc_set_channel_value(uint8_t channel, uint16_t value) {
    if (channel < 16) {
        mock_adc.channel_values[channel] = value;
    }
}

// Mock I2C functions
void mock_i2c_init(uint8_t peripheral) {
    if (peripheral < 2) {
        mock_i2c_devices[peripheral].initialized = true;
    }
}

int mock_i2c_write(uint8_t peripheral, uint8_t addr, const uint8_t *data, size_t len) {
    if (peripheral >= 2 || !mock_i2c_devices[peripheral].initialized || !data || len == 0) {
        return -1;
    }
    
    mock_i2c_t *device = &mock_i2c_devices[peripheral];
    device->device_address = addr;
    
    if (len <= sizeof(device->tx_buffer)) {
        memcpy(device->tx_buffer, data, len);
        device->tx_length = len;
    }
    
    return device->transaction_success ? 0 : -1;
}

int mock_i2c_read(uint8_t peripheral, uint8_t addr, uint8_t *data, size_t len) {
    if (peripheral >= 2 || !mock_i2c_devices[peripheral].initialized || !data || len == 0) {
        return -1;
    }
    
    mock_i2c_t *device = &mock_i2c_devices[peripheral];
    device->device_address = addr;
    
    size_t copy_len = len < device->rx_length ? len : device->rx_length;
    if (copy_len > 0) {
        memcpy(data, device->rx_buffer, copy_len);
    }
    
    return device->transaction_success ? 0 : -1;
}

void mock_i2c_set_device_response(uint8_t peripheral, uint8_t addr, const uint8_t *response, size_t len) {
    if (peripheral >= 2 || !response || len == 0) {
        return;
    }
    
    mock_i2c_t *device = &mock_i2c_devices[peripheral];
    if (len <= sizeof(device->rx_buffer)) {
        memcpy(device->rx_buffer, response, len);
        device->rx_length = len;
        device->transaction_success = true;
    }
}

// Mock UART functions
void mock_uart_init(void) {
    mock_uart.initialized = true;
}

void mock_uart_transmit(const uint8_t *data, size_t len) {
    if (!mock_uart.initialized || !data || len == 0) {
        return;
    }
    
    size_t available = sizeof(mock_uart.tx_buffer) - mock_uart.tx_length;
    size_t copy_len = len < available ? len : available;
    
    if (copy_len > 0) {
        memcpy(mock_uart.tx_buffer + mock_uart.tx_length, data, copy_len);
        mock_uart.tx_length += copy_len;
    }
}

size_t mock_uart_receive(uint8_t *data, size_t max_len) {
    if (!mock_uart.initialized || !data || max_len == 0) {
        return 0;
    }
    
    size_t copy_len = max_len < mock_uart.rx_length ? max_len : mock_uart.rx_length;
    if (copy_len > 0) {
        memcpy(data, mock_uart.rx_buffer, copy_len);
        // Shift remaining data
        memmove(mock_uart.rx_buffer, mock_uart.rx_buffer + copy_len, 
                mock_uart.rx_length - copy_len);
        mock_uart.rx_length -= copy_len;
    }
    
    return copy_len;
}

void mock_uart_inject_data(const uint8_t *data, size_t len) {
    if (!data || len == 0) {
        return;
    }
    
    size_t available = sizeof(mock_uart.rx_buffer) - mock_uart.rx_length;
    size_t copy_len = len < available ? len : available;
    
    if (copy_len > 0) {
        memcpy(mock_uart.rx_buffer + mock_uart.rx_length, data, copy_len);
        mock_uart.rx_length += copy_len;
    }
}

// Mock PWM functions
void mock_pwm_init(void) {
    // Already initialized by reset
}

void mock_pwm_set_duty_cycle(uint8_t channel, uint32_t duty) {
    if (channel < 4) {
        mock_pwm_channels[channel].duty_cycle = duty;
    }
}

void mock_pwm_set_frequency(uint8_t channel, uint32_t frequency) {
    if (channel < 4) {
        mock_pwm_channels[channel].frequency = frequency;
    }
}

void mock_pwm_enable(uint8_t channel) {
    if (channel < 4) {
        mock_pwm_channels[channel].enabled = true;
    }
}

void mock_pwm_disable(uint8_t channel) {
    if (channel < 4) {
        mock_pwm_channels[channel].enabled = false;
    }
}

// Mock delay functions
void mock_delay_ms(uint32_t ms) {
    mock_system_ticks += ms;
}

void mock_delay_us(uint32_t us) {
    mock_system_ticks += (us + 999) / 1000; // Round up to milliseconds
}

// Mock system time
uint32_t mock_get_tick(void) {
    return mock_system_ticks;
}

void mock_advance_time(uint32_t ms) {
    mock_system_ticks += ms;
}

// Test utilities
void mock_print_gpio_state(void) {
    printf("GPIO State:\n");
    for (int port = 0; port < 3; port++) {
        printf("  Port %c: ", 'A' + port);
        for (int pin = 0; pin < 16; pin++) {
            printf("%d", mock_gpio_ports[port].pin_states[pin]);
        }
        printf("\n");
    }
}

void mock_print_i2c_transactions(void) {
    printf("I2C Transactions:\n");
    for (int i = 0; i < 2; i++) {
        printf("  I2C%d: addr=0x%02X, tx_len=%zu, rx_len=%zu\n", 
               i+1, mock_i2c_devices[i].device_address,
               mock_i2c_devices[i].tx_length, mock_i2c_devices[i].rx_length);
    }
}

void mock_print_uart_data(void) {
    printf("UART Data:\n");
    printf("  TX (%zu bytes): ", mock_uart.tx_length);
    for (size_t i = 0; i < mock_uart.tx_length; i++) {
        printf("%02X ", mock_uart.tx_buffer[i]);
    }
    printf("\n");
    printf("  RX (%zu bytes): ", mock_uart.rx_length);
    for (size_t i = 0; i < mock_uart.rx_length; i++) {
        printf("%02X ", mock_uart.rx_buffer[i]);
    }
    printf("\n");
}