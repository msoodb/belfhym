/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_I2C_H
#define BLFM_I2C_H

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/*                          PUBLIC CONSTANTS                                 */
/* ========================================================================== */

#define I2C_MAX_INSTANCES               2
#define I2C_TIMEOUT_VALUE               10000

/* MPU6050 Register Addresses */
#define MPU6050_PWR_MGMT_1              0x6B
#define MPU6050_SMPLRT_DIV              0x19
#define MPU6050_CONFIG                  0x1A
#define MPU6050_GYRO_CONFIG             0x1B
#define MPU6050_ACCEL_CONFIG            0x1C
#define MPU6050_ACCEL_XOUT_H            0x3B
#define MPU6050_ACCEL_XOUT_L            0x3C
#define MPU6050_ACCEL_YOUT_H            0x3D
#define MPU6050_ACCEL_YOUT_L            0x3E
#define MPU6050_ACCEL_ZOUT_H            0x3F
#define MPU6050_ACCEL_ZOUT_L            0x40
#define MPU6050_TEMP_OUT_H              0x41
#define MPU6050_TEMP_OUT_L              0x42
#define MPU6050_GYRO_XOUT_H             0x43
#define MPU6050_GYRO_XOUT_L             0x44
#define MPU6050_GYRO_YOUT_H             0x45
#define MPU6050_GYRO_YOUT_L             0x46
#define MPU6050_GYRO_ZOUT_H             0x47
#define MPU6050_GYRO_ZOUT_L             0x48

/* Common I2C Device Addresses */
#define MPU6050_ADDRESS_AD0_LOW         0x68
#define MPU6050_ADDRESS_AD0_HIGH        0x69
#define BMP180_ADDRESS                  0x77
#define DS1307_ADDRESS                  0x68
#define EEPROM_24C02_ADDRESS            0x50

/* ========================================================================== */
/*                          PUBLIC ENUMERATIONS                              */
/* ========================================================================== */

/**
 * @brief I2C instances
 */
typedef enum {
    BLFM_I2C1 = 0,
    BLFM_I2C2 = 1
} blfm_i2c_instance_t;

/**
 * @brief I2C status codes
 */
typedef enum {
    BLFM_I2C_OK = 0,
    BLFM_I2C_ERROR_INVALID_PARAM,
    BLFM_I2C_ERROR_NOT_INITIALIZED,
    BLFM_I2C_ERROR_ALREADY_INITIALIZED,
    BLFM_I2C_ERROR_TIMEOUT,
    BLFM_I2C_ERROR_NO_DEVICE,
    BLFM_I2C_ERROR_BUSY,
    BLFM_I2C_ERROR_ARBITRATION_LOST,
    BLFM_I2C_ERROR_BUS_ERROR
} blfm_i2c_status_t;

/**
 * @brief I2C addressing mode
 */
typedef enum {
    BLFM_I2C_ADDRESSINGMODE_7BIT = 0,
    BLFM_I2C_ADDRESSINGMODE_10BIT = 1
} blfm_i2c_addressing_mode_t;

/**
 * @brief I2C duty cycle for fast mode
 */
typedef enum {
    BLFM_I2C_DUTYCYCLE_2 = 0,
    BLFM_I2C_DUTYCYCLE_16_9 = 1
} blfm_i2c_duty_cycle_t;

/* ========================================================================== */
/*                          PUBLIC STRUCTURES                                */
/* ========================================================================== */

/**
 * @brief I2C configuration structure
 */
typedef struct {
    uint32_t clock_speed;                           /* Clock speed (100000 or 400000) */
    blfm_i2c_addressing_mode_t addressing_mode;     /* 7-bit or 10-bit addressing */
    blfm_i2c_duty_cycle_t duty_cycle;               /* Duty cycle for fast mode */
    uint16_t own_address;                           /* Own device address */
    bool ack_enable;                                /* ACK enable */
} blfm_i2c_config_t;

/* ========================================================================== */
/*                          PUBLIC FUNCTION PROTOTYPES                       */
/* ========================================================================== */

/* ========================================================================== */
/*                          DRIVER LAYER FUNCTIONS                           */
/* ========================================================================== */

/**
 * @brief Initialize I2C instance
 * @param instance I2C instance to initialize
 * @param config Configuration parameters
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_init(blfm_i2c_instance_t instance, const blfm_i2c_config_t *config);

/**
 * @brief Deinitialize I2C instance
 * @param instance I2C instance to deinitialize
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_deinit(blfm_i2c_instance_t instance);

/**
 * @brief Check if device is present on I2C bus
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param trials Number of trials
 * @return BLFM_I2C_OK if device present, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_is_device_ready(blfm_i2c_instance_t instance, uint8_t device_address, uint32_t trials);

/**
 * @brief Write data to I2C device (blocking)
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param data Data to write
 * @param size Number of bytes to write
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_write(blfm_i2c_instance_t instance, uint8_t device_address, const uint8_t *data, uint16_t size);

/**
 * @brief Read data from I2C device (blocking)
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param data Buffer for read data
 * @param size Number of bytes to read
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_read(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t *data, uint16_t size);

/**
 * @brief Write then read from I2C device (blocking)
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param write_data Data to write
 * @param write_size Number of bytes to write
 * @param read_data Buffer for read data
 * @param read_size Number of bytes to read
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_write_read(blfm_i2c_instance_t instance, uint8_t device_address, const uint8_t *write_data, uint16_t write_size, uint8_t *read_data, uint16_t read_size);

/* ========================================================================== */
/*                          PROTOCOL LAYER FUNCTIONS                         */
/* ========================================================================== */

/**
 * @brief Write register to I2C device
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param reg_address Register address
 * @param value Value to write
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_write_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t value);

/**
 * @brief Read register from I2C device
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param reg_address Register address
 * @param value Pointer to store read value
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_read_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t *value);

/**
 * @brief Write multiple registers
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param start_reg Starting register address
 * @param data Data to write
 * @param length Number of bytes to write
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_write_registers(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t start_reg, const uint8_t *data, uint16_t length);

/**
 * @brief Read multiple registers
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param start_reg Starting register address
 * @param data Buffer for read data
 * @param length Number of bytes to read
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_read_registers(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t start_reg, uint8_t *data, uint16_t length);

/**
 * @brief Modify bits in I2C device register
 * @param instance I2C instance
 * @param device_address Device address (7-bit, left-shifted)
 * @param reg_address Register address
 * @param mask Bit mask
 * @param value New value for masked bits
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_modify_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t mask, uint8_t value);

/* ========================================================================== */
/*                          COMMON SENSOR PROTOCOLS                          */
/* ========================================================================== */

/**
 * @brief MPU6050 initialization
 * @param instance I2C instance
 * @param device_address MPU6050 device address
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_mpu6050_init(blfm_i2c_instance_t instance, uint8_t device_address);

/**
 * @brief MPU6050 read accelerometer data
 * @param instance I2C instance
 * @param device_address MPU6050 device address
 * @param accel_x Pointer to store X acceleration
 * @param accel_y Pointer to store Y acceleration
 * @param accel_z Pointer to store Z acceleration
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_mpu6050_read_accel(blfm_i2c_instance_t instance, uint8_t device_address, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);

/**
 * @brief MPU6050 read gyroscope data
 * @param instance I2C instance
 * @param device_address MPU6050 device address
 * @param gyro_x Pointer to store X angular velocity
 * @param gyro_y Pointer to store Y angular velocity
 * @param gyro_z Pointer to store Z angular velocity
 * @return BLFM_I2C_OK on success, error code otherwise
 */
blfm_i2c_status_t blfm_i2c_mpu6050_read_gyro(blfm_i2c_instance_t instance, uint8_t device_address, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);

/* ========================================================================== */
/*                          CONVENIENCE MACROS                               */
/* ========================================================================== */

/**
 * @brief Default I2C configuration for standard mode (100kHz)
 */
#define BLFM_I2C_CONFIG_STANDARD() { \
    .clock_speed = 100000, \
    .addressing_mode = BLFM_I2C_ADDRESSINGMODE_7BIT, \
    .duty_cycle = BLFM_I2C_DUTYCYCLE_2, \
    .own_address = 0x00, \
    .ack_enable = true \
}

/**
 * @brief I2C configuration for fast mode (400kHz)
 */
#define BLFM_I2C_CONFIG_FAST() { \
    .clock_speed = 400000, \
    .addressing_mode = BLFM_I2C_ADDRESSINGMODE_7BIT, \
    .duty_cycle = BLFM_I2C_DUTYCYCLE_2, \
    .own_address = 0x00, \
    .ack_enable = true \
}

/**
 * @brief Convert 7-bit address to transmission format
 */
#define I2C_ADDRESS(addr)               ((addr) << 1)

#endif /* BLFM_I2C_H */