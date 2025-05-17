
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_SENSOR_HUB_H
#define BLFM_SENSOR_HUB_H

#include "blfm_types.h"
#include <stdbool.h>

void blfm_sensor_hub_init(void);
void blfm_sensor_hub_start(void);
void blfm_sensor_hub_poll(void);
bool blfm_sensor_hub_get_data(blfm_sensor_data_t *out_data);

#endif // BLFM_SENSOR_HUB_H

