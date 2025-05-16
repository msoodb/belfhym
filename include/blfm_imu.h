
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

#include "blfm_types.h"

void blfm_imu_init(void);
void blfm_imu_read(blfm_imu_data_t* data);

#endif // BLFM_IMU_H

