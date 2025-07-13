
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_I2C1_H
#define BLFM_I2C1_H

#include <stdint.h>
#include <stddef.h>

void blfm_i2c1_init(void);
int blfm_i2c1_write(uint8_t addr, const uint8_t *data, size_t len);
int blfm_i2c1_write_byte(uint8_t addr, uint8_t reg, uint8_t data);
int blfm_i2c1_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len);

#endif // BLFM_I2C1_H
