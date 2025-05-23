
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

void blfm_sensor_hub_init(void);
bool blfm_sensor_hub_read(blfm_sensor_data_t *out);

#endif // BLFM_SENSOR_HUB_H

