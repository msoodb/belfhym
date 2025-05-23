
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

#include "blfm_types.h"
#include <stdbool.h>

void blfm_ultrasonic_init(void);
bool blfm_ultrasonic_read(blfm_ultrasonic_data_t *data);

#endif // BLFM_ULTRASONIC_H

