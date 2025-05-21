
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
 * @brief Initialize I2C1 peripheral.
 */
void blfm_i2c_init(void);

/**
 * @brief Write a single byte to an I2C device.
 * 
 * @param addr 7-bit device address
 * @param reg Register address
 * @param data Byte to write
 * @return 0 on success, -1 on failure
 */
int blfm_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data);

/**
 * @brief Read multiple bytes from an I2C device.
 * 
 * @param addr 7-bit device address
 * @param reg Starting register
 * @param buf Destination buffer
 * @param len Number of bytes to read
 * @return 0 on success, -1 on failure
 */
int blfm_i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len);

int blfm_i2c_write_byte_simple(uint8_t addr, uint8_t data);

#endif // BLFM_I2C_H
