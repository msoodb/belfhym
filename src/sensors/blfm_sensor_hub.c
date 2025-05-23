
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_sensor_hub.h"
#include "blfm_ultrasonic.h"
#include "blfm_temperature.h"
#include "blfm_imu.h"

#include <stdbool.h>

void blfm_sensor_hub_init(void) {
  blfm_ultrasonic_init();
  //blfm_imu_init();
  //blfm_temperature_init();
}

bool blfm_sensor_hub_read(blfm_sensor_data_t *out) {
 
  if (!out) return false;


  bool ok_ultrasonic = blfm_ultrasonic_read(&out->ultrasonic);
  /*bool ok_imu = true; //blfm_imu_read(&out->imu);
  bool ok_temperature = true; //blfm_temperature_read(&out->temperature);

  return  ok_ultrasonic && ok_imu && ok_temperature;
  */
  
  //out->ultrasonic.distance_mm = 100;
  return true;

}
