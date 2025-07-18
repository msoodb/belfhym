
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_led.h"
#include "blfm_motor.h"
#include "blfm_oled.h"
#include "blfm_servomotor.h"
#include "blfm_actuator_hub.h"
#include "blfm_types.h"

void blfm_actuator_hub_init(void) {
  blfm_led_init();
  blfm_motor_init();
  blfm_oled_init();
  blfm_servomotor_init();
}

void blfm_actuator_hub_apply(const blfm_actuator_command_t *cmd) {
  if (!cmd)
    return;

  blfm_led_apply(&cmd->led);
  blfm_motor_apply(&cmd->motor);
  blfm_oled_apply(&cmd->oled);
  blfm_servomotor_apply(&cmd->servo);
}
