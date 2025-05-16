
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_imu.h"

void blfm_imu_init(void) {
  // Initialize IMU sensor (mock implementation)
}

void blfm_imu_read(blfm_imu_data_t *data) {
  data->acc_x = 0;
  data->acc_y = 0;
  data->acc_z = 0;
  data->gyro_x = 0;
  data->gyro_y = 0;
  data->gyro_z = 0;
}
