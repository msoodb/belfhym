
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"

#if BLFM_ENABLED_LED
#include "blfm_led.h"
#endif

#if BLFM_ENABLED_MOTOR
#include "blfm_motor.h"
#endif

#if BLFM_ENABLED_DISPLAY
#include "blfm_display.h"
#endif

#if BLFM_ENABLED_OLED
#include "blfm_oled.h"
#endif

#if BLFM_ENABLED_ALARM
#include "blfm_alarm.h"
#endif

#if BLFM_ENABLED_RADIO
#include "blfm_radio.h"
#endif

#if BLFM_ENABLED_SERVO
#include "blfm_servomotor.h"
#endif

#include "blfm_actuator_hub.h"
#include "blfm_types.h"

void blfm_actuator_hub_init(void) {
#if BLFM_ENABLED_LED
  blfm_led_init();
#endif

#if BLFM_ENABLED_MOTOR
  blfm_motor_init();
#endif

#if BLFM_ENABLED_DISPLAY
  blfm_display_init();
#endif

#if BLFM_ENABLED_OLED
  blfm_oled_init();
#endif

#if BLFM_ENABLED_SERVO
  blfm_servomotor_init();
  blfm_servomotor_set_type(BLFM_SERVO_TYPE_RADAR);
#endif

#if BLFM_ENABLED_ALARM
  blfm_alarm_init();
#endif

#if BLFM_ENABLED_RADIO
  blfm_radio_init();
#endif
}

void blfm_actuator_hub_apply(const blfm_actuator_command_t *cmd) {
  if (!cmd)
    return;

#if BLFM_ENABLED_LED
  blfm_led_apply(&cmd->led);
#endif

#if BLFM_ENABLED_MOTOR
  blfm_motor_apply(&cmd->motor);
#endif

#if BLFM_ENABLED_DISPLAY
  blfm_display_apply(&cmd->display);
#endif

#if BLFM_ENABLED_OLED
  blfm_oled_apply(&cmd->oled);
#endif

#if BLFM_ENABLED_SERVO
  blfm_servomotor_apply(&cmd->servo);
#endif

#if BLFM_ENABLED_ALARM
  blfm_alarm_apply(&cmd->alarm);
#endif

#if BLFM_ENABLED_RADIO
  blfm_radio_apply(&cmd->radio);
#endif
}
