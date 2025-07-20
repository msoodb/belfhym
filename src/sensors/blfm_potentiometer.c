/**
 * Belfhym Potentiometer Module
 * Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * GPLv3
 */

#include "blfm_config.h"
#if BLFM_ENABLED_POTENTIOMETER

#include "blfm_potentiometer.h"
#include "blfm_adc.h"
#include "blfm_pins.h"
#include <stdbool.h>

void blfm_potentiometer_init(void) {
  // Just init ADC for now (PA6 pin)
  blfm_adc_init();
}

bool blfm_potentiometer_read(blfm_potentiometer_data_t *data) {
  if (!data)
    return false;

  uint16_t raw_value = 0;
  int ret = blfm_adc_read(BLFM_POTENTIOMETER_ADC_CHANNEL, &raw_value);
  if (ret != 0)
    return false;

  data->raw_value = raw_value;
  return true;
}

#endif /* BLFM_ENABLED_POTENTIOMETER */
