
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_ULTRASONIC_H
#define BLFM_ULTRASONIC_H

#include <stdint.h>

void blfm_ultrasonic_init(void);
uint16_t blfm_ultrasonic_get_distance(void);

#endif // BLFM_ULTRASONIC_H

