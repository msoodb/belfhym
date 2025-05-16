
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_MOTOR_H
#define BLFM_MOTOR_H

#include <stdint.h>

void blfm_motor_init(void);
void blfm_motor_set_speed(uint8_t left, uint8_t right);

#endif // BLFM_MOTOR_H

