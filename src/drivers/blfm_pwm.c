
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_pwm.h"

void blfm_pwm_init(uint8_t channel) {
  (void) channel;
  // TODO: configure PWM timer for the specified channel
}

void blfm_pwm_set_duty(uint8_t channel, uint16_t duty) {
  (void) channel;
  (void) duty;
  // TODO: set duty cycle for the specified PWM channel
}
