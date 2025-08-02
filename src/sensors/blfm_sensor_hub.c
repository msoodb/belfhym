
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"
#include "blfm_sensor_hub.h"

#if BLFM_ENABLED_ULTRASONIC
#include "blfm_ultrasonic.h"
#endif


#if BLFM_ENABLED_TEMPERATURE
#include "blfm_temperature.h"
#endif

#include <stdbool.h>

void blfm_sensor_hub_init(void) {
#if BLFM_ENABLED_ULTRASONIC
  blfm_ultrasonic_init();
#endif


#if BLFM_ENABLED_TEMPERATURE
  blfm_temperature_init();
#endif
}

bool blfm_sensor_hub_read(blfm_sensor_data_t *out) {
  if (!out) {
    return false;
  }

  bool ok = true;

#if BLFM_ENABLED_ULTRASONIC
  ok &= blfm_ultrasonic_read(&out->ultrasonic);
#endif


#if BLFM_ENABLED_TEMPERATURE
  ok &= blfm_temperature_read(&out->temperature);
#endif

  return ok;
}
