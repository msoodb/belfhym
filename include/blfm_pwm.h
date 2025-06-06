
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_PWM_H
#define BLFM_PWM_H

#include <stdint.h>

void blfm_pwm_init();
void blfm_pwm_set_pulse_us(uint16_t us);

#endif // BLFM_PWM_H

