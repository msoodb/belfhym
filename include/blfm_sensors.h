
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_SENSORS_H
#define BLFM_SENSORS_H

#include "blfm_types.h"
#include <stdint.h>
#include <stdbool.h>

void blfm_sensor_hub_init(void);
bool blfm_sensor_hub_read(blfm_sensor_data_t *out);

#endif
