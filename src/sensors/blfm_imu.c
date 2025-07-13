
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/* MPU-6050 */

#include "blfm_imu.h"
#include "blfm_i2c1.h"

#define MPU6050_ADDR          0x68
#define MPU6050_REG_PWR_MGMT  0x6B
#define MPU6050_REG_ACCEL_X   0x3B

void blfm_imu_init(void) {
  blfm_i2c1_write_byte(MPU6050_ADDR, MPU6050_REG_PWR_MGMT, 0x00);
}

bool blfm_imu_read(blfm_imu_data_t *data) {
  if (!data) return false;

  uint8_t raw[14];
  blfm_i2c1_read_bytes(MPU6050_ADDR, MPU6050_REG_ACCEL_X, raw, 14);

  data->acc_x = (int16_t)(raw[0] << 8 | raw[1]);
  data->acc_y = (int16_t)(raw[2] << 8 | raw[3]);
  data->acc_z = (int16_t)(raw[4] << 8 | raw[5]);

  data->gyro_x = (int16_t)(raw[8] << 8 | raw[9]);
  data->gyro_y = (int16_t)(raw[10] << 8 | raw[11]);
  data->gyro_z = (int16_t)(raw[12] << 8 | raw[13]);

  return true;
}
