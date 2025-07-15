# I2C Module Usage Guide

This document explains how to use the refactored I2C module that supports both I2C1 and I2C2 peripherals.

## Overview

The new I2C module provides a unified interface for both I2C1 and I2C2 peripherals with:
- Standardized error handling
- Configurable speed and timeout settings
- Support for both peripherals with pin definitions
- Device presence detection
- Helper functions for common operations

## Pin Assignments

### I2C1 (Currently used by OLED and IMU)
- **SCL**: PB6
- **SDA**: PB7

### I2C2 (Available for additional devices)
- **SCL**: PB10
- **SDA**: PB11

## Basic Usage

### 1. Initialization

```c
#include "blfm_i2c.h"

// Initialize I2C1 with default settings (100kHz, 100ms timeout)
blfm_i2c_error_t result = blfm_i2c_init(BLFM_I2C1, NULL);
if (result != BLFM_I2C_OK) {
    // Handle initialization error
}

// Initialize I2C2 with custom settings
blfm_i2c_config_t config = {
    .speed_hz = 400000,  // 400kHz fast mode
    .timeout_ms = 50     // 50ms timeout
};
result = blfm_i2c_init(BLFM_I2C2, &config);
```

### 2. Writing Data

```c
// Write raw data to device
uint8_t data[] = {0x12, 0x34, 0x56};
blfm_i2c_error_t result = blfm_i2c_write(BLFM_I2C1, 0x3C, data, sizeof(data));

// Write single byte to device register
result = blfm_i2c_write_byte(BLFM_I2C1, 0x68, 0x6B, 0x00);
```

### 3. Reading Data

```c
// Read raw data from device
uint8_t buffer[10];
blfm_i2c_error_t result = blfm_i2c_read(BLFM_I2C1, 0x68, buffer, sizeof(buffer));

// Read bytes from device register
result = blfm_i2c_read_bytes(BLFM_I2C1, 0x68, 0x3B, buffer, 14);
```

### 4. Device Detection

```c
// Check if device is present on the bus
blfm_i2c_error_t result = blfm_i2c_device_present(BLFM_I2C1, 0x68);
if (result == BLFM_I2C_OK) {
    // Device is present and responding
} else {
    // Device not found or not responding
}
```

### 5. Error Handling

```c
blfm_i2c_error_t result = blfm_i2c_write(BLFM_I2C1, 0x3C, data, len);
switch (result) {
    case BLFM_I2C_OK:
        // Success
        break;
    case BLFM_I2C_ERR_TIMEOUT:
        // Communication timeout
        break;
    case BLFM_I2C_ERR_INVALID_PERIPHERAL:
        // Invalid peripheral selected
        break;
    case BLFM_I2C_ERR_NOT_INITIALIZED:
        // Peripheral not initialized
        break;
    case BLFM_I2C_ERR_NULL_PTR:
        // Null pointer passed
        break;
    default:
        // Other error
        break;
}
```

## Example: Adding a New I2C Device

Here's how to add a new I2C sensor on I2C2:

```c
// File: src/sensors/blfm_new_sensor.c
#include "blfm_i2c.h"

#define NEW_SENSOR_I2C_ADDR 0x48
#define NEW_SENSOR_REG_DATA 0x00

void blfm_new_sensor_init(void) {
    // Initialize I2C2 if not already done
    blfm_i2c_config_t config = {
        .speed_hz = 100000,  // 100kHz
        .timeout_ms = 100
    };
    blfm_i2c_init(BLFM_I2C2, &config);
    
    // Check if device is present
    if (blfm_i2c_device_present(BLFM_I2C2, NEW_SENSOR_I2C_ADDR) != BLFM_I2C_OK) {
        // Handle device not found
        return;
    }
    
    // Configure device
    blfm_i2c_write_byte(BLFM_I2C2, NEW_SENSOR_I2C_ADDR, 0x01, 0x80);
}

bool blfm_new_sensor_read(uint16_t *value) {
    if (!value) return false;
    
    uint8_t data[2];
    blfm_i2c_error_t result = blfm_i2c_read_bytes(BLFM_I2C2, NEW_SENSOR_I2C_ADDR, 
                                                   NEW_SENSOR_REG_DATA, data, 2);
    
    if (result == BLFM_I2C_OK) {
        *value = (data[0] << 8) | data[1];
        return true;
    }
    
    return false;
}
```

## Migration from Old I2C1 Module

If you have existing code using the old `blfm_i2c1.h` interface:

### Before:
```c
#include "blfm_i2c1.h"
blfm_i2c1_init();
blfm_i2c1_write(0x3C, data, len);
blfm_i2c1_write_byte(0x68, 0x6B, 0x00);
blfm_i2c1_read_bytes(0x68, 0x3B, buffer, 14);
```

### After:
```c
#include "blfm_i2c.h"
blfm_i2c_init(BLFM_I2C1, NULL);
blfm_i2c_write(BLFM_I2C1, 0x3C, data, len);
blfm_i2c_write_byte(BLFM_I2C1, 0x68, 0x6B, 0x00);
blfm_i2c_read_bytes(BLFM_I2C1, 0x68, 0x3B, buffer, 14);
```

## Board Initialization

The board initialization automatically initializes I2C1 with default settings. If you need I2C2, initialize it manually in your module's init function:

```c
// In your module's init function
void blfm_my_module_init(void) {
    // Initialize I2C2 for this module
    blfm_i2c_init(BLFM_I2C2, NULL);
    
    // Rest of initialization...
}
```

## Benefits of the New I2C Module

1. **Unified Interface**: Same API for both I2C1 and I2C2
2. **Better Error Handling**: Standardized error codes and proper error propagation
3. **Configurable**: Speed and timeout settings can be customized
4. **Device Detection**: Built-in function to check device presence
5. **Maintainable**: Single module to maintain instead of separate I2C1/I2C2 modules
6. **Future-Ready**: Easy to add more I2C peripherals if needed

## Notes

- The GPIO configuration is automatically handled by the I2C module
- Pin assignments are defined in `blfm_pins.h`
- Both I2C peripherals can operate simultaneously
- The module handles proper clock enabling and peripheral configuration
- Error handling is consistent across all functions