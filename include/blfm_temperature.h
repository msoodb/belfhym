
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_TEMPERATURE_H
#define BLFM_TEMPERATURE_H

#include <stdbool.h>
#include <stdint.h>

void blfm_temperature_init(void);
bool blfm_temperature_read(int32_t *temperature_mC);

#endif // BLFM_TEMPERATURE_H
