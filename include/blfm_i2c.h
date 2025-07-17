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
#include <stddef.h>

/**
 * @brief I2C error codes
 */
typedef enum {
  BLFM_I2C_OK = 0,
  BLFM_I2C_ERR_NULL_PTR,
  BLFM_I2C_ERR_TIMEOUT,
  BLFM_I2C_ERR_NOT_INITIALIZED
} blfm_i2c_error_t;

/**
 * @brief I2C configuration structure
 */
typedef struct {
  uint32_t speed_hz;      // I2C speed in Hz (100000 for 100kHz, 400000 for 400kHz)
  uint8_t timeout_ms;     // Timeout in milliseconds
} blfm_i2c_config_t;

/**
 * @brief Initialize I2C1 peripheral (PB6=SCL, PB7=SDA)
 * @param config Configuration parameters (NULL for default config)
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_init(const blfm_i2c_config_t *config);

/**
 * @brief Deinitialize I2C1 peripheral
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_deinit(void);

/**
 * @brief Write data to I2C device
 * @param addr 7-bit device address
 * @param data Pointer to data buffer
 * @param len Number of bytes to write
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_write(uint8_t addr, const uint8_t *data, size_t len);

/**
 * @brief Read data from I2C device
 * @param addr 7-bit device address
 * @param data Pointer to data buffer
 * @param len Number of bytes to read
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_read(uint8_t addr, uint8_t *data, size_t len);

/**
 * @brief Write single byte to I2C device register
 * @param addr 7-bit device address
 * @param reg Register address
 * @param data Data byte to write
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data);

/**
 * @brief Read bytes from I2C device register
 * @param addr 7-bit device address
 * @param reg Register address
 * @param data Pointer to data buffer
 * @param len Number of bytes to read
 * @return BLFM_I2C_OK on success, error code on failure
 */
blfm_i2c_error_t blfm_i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);

/**
 * @brief Check if I2C device is present on the bus
 * @param addr 7-bit device address
 * @return BLFM_I2C_OK if device responds, error code otherwise
 */
blfm_i2c_error_t blfm_i2c_device_present(uint8_t addr);

#endif // BLFM_I2C_H