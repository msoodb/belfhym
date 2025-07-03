#ifndef BLFM_POTENTIOMETER_H
#define BLFM_POTENTIOMETER_H

#include <stdint.h>
#include "blfm_types.h"

/**
 * Initialize potentiometer module (ADC setup etc.)
 */
void blfm_potentiometer_init(void);

/**
 * Read the potentiometer value (0-4095).
 * Returns -1 on error.
 */
bool blfm_potentiometer_read(blfm_potentiometer_data_t *data);

#endif /* BLFM_POTENTIOMETER_H */
