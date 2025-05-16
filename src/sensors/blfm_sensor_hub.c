
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_sensor_hub.h"
#include "blfm_imu.h"
#include "blfm_ultrasonic.h"

void blfm_sensor_hub_init(void) {
  blfm_ultrasonic_init();
  blfm_imu_init();
}

void blfm_sensor_hub_start(void) {
  // Start sensor tasks if needed
}
