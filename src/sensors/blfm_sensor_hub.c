
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
#include "blfm_potentiometer.h"
//#include "blfm_temperature.h"

#include <stdbool.h>

void blfm_sensor_hub_init(void) {
  blfm_ultrasonic_init();
  //blfm_potentiometer_init();
  // blfm_temperature_init();
}

bool blfm_sensor_hub_read(blfm_sensor_data_t *out) {
  if (!out) {
    return false;
  }

  bool ok = true;

  ok &= blfm_ultrasonic_read(&out->ultrasonic);
  //ok &= blfm_potentiometer_read(&out->potentiometer);
  // ok &= blfm_temperature_read(&out->temperature);

  return ok;
}
