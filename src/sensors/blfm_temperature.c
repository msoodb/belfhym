
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_temperature.h"
#include "blfm_adc.h"
#include "blfm_pins.h"
#include <stdbool.h>

void blfm_temperature_init(void) {
  // Initialize ADC, if needed
#if BLFM_TEMPRATURE_DISABLED
  return;
#endif

}

bool blfm_temperature_read(blfm_temperature_data_t *temp) {
#if BLFM_TEMPRATURE_DISABLED
  return true;
#endif

  if (!temp)
    return false;

  uint16_t adc_value = 0;
  if (blfm_adc_read(BLFM_TEMP_SENSOR_PIN, &adc_value) != 0) {
    return false;
  }

  // Convert ADC value to millivolts using integer math
  // (adc_value * 3300) / 4095
  // To maintain precision, use 64-bit math if needed
  uint32_t voltage_mv = ((uint32_t)adc_value * 3300 + 2047) / 4095;

  // Temperature in milli°C: (voltage_mv - 500) * 1000 / 10 = (voltage_mv - 500)
  // * 100
  int32_t temperature_mc = ((int32_t)voltage_mv - 500) * 100;

  temp->temperature_mc = temperature_mc;
  return true;
}
