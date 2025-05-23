
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_temperature.h"
#include "blfm_i2c.h"
#include <stdbool.h>

#define TMP102_ADDR      0x48
#define TMP102_TEMP_REG  0x00

void blfm_temperature_init(void) {
}

bool blfm_temperature_read(blfm_temperature_data_t *temp) {
  if (!temp) return false;

  uint8_t raw_data[2];
  if (blfm_i2c_read_bytes(TMP102_ADDR, TMP102_TEMP_REG, raw_data, 2) != 0) {
    return false;
  }

  int16_t raw_temp = (raw_data[0] << 8) | raw_data[1];
  raw_temp >>= 4;

  if (raw_temp & 0x800) {
    raw_temp |= 0xF000;
  }

  temp->temperature_mc = (int32_t)raw_temp * 625 / 10;
  return true;
}
