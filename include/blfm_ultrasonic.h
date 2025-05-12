#ifndef BLFM_ULTRASONIC_H
#define BLFM_ULTRASONIC_H

#include <stdint.h>

void blfm_ultrasonic_init(void);
uint16_t blfm_ultrasonic_get_distance_mm(void);

#endif
