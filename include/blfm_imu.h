/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_IMU_H
#define BLFM_IMU_H

void blfm_imu_init(void);            // Initialize IMU sensors
void blfm_imu_read(void);       // Read IMU data
void blfm_imu_get_orientation(void);    // Process IMU data (sensor fusion)

#endif // BLFM_IMU_H
