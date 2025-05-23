
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_actuator_hub.h"
#include "blfm_motor.h"
#include "blfm_display.h"
#include "blfm_led.h"
#include "blfm_alarm.h"
#include "blfm_radio.h"

void blfm_actuator_hub_init(void) {
  blfm_motor_init();
  //blfm_display_init();
  blfm_led_init();
  blfm_alarm_init();
  blfm_radio_init();
}

void blfm_actuator_hub_apply(const blfm_actuator_command_t *cmd) {
  if (!cmd) return;

  blfm_motor_apply(&cmd->motor);
  //blfm_display_apply(&cmd->display);
  blfm_led_apply(cmd->led);
  blfm_alarm_apply(&cmd->alarm);
  blfm_radio_apply(&cmd->radio);
}

