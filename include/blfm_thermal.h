#ifndef BLFM_THERMAL_H
#define BLFM_THERMAL_H

#include <stdint.h>

void blfm_thermal_init(void);
uint16_t blfm_thermal_get_value_raw(void);
int16_t blfm_thermal_get_temperature_dc(void);

#endif // BLFM_THERMAL_H
