
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


#if BLFM_ENABLED_OLED
#include "blfm_oled.h"
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


#if BLFM_ENABLED_OLED
  blfm_oled_init();
#endif

#if BLFM_ENABLED_SERVO
  blfm_servomotor_init();
  // Set servo types for enabled servos only
#if BLFM_ENABLED_SERVO1
  blfm_servomotor_set_type(0, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo1 - PA0
#endif
#if BLFM_ENABLED_SERVO2
  blfm_servomotor_set_type(1, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo2 - PA1
#endif
#if BLFM_ENABLED_SERVO3
  blfm_servomotor_set_type(2, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo3 - PA2
#endif
#if BLFM_ENABLED_SERVO4
  blfm_servomotor_set_type(3, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo4 - PA3
#endif
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


#if BLFM_ENABLED_OLED
  blfm_oled_apply(&cmd->oled);
#endif

#if BLFM_ENABLED_SERVO
  // Apply commands to enabled servos only
#if BLFM_ENABLED_SERVO1
  blfm_servomotor_apply(0, &cmd->servo1);
#endif
#if BLFM_ENABLED_SERVO2
  blfm_servomotor_apply(1, &cmd->servo2);
#endif
#if BLFM_ENABLED_SERVO3
  blfm_servomotor_apply(2, &cmd->servo3);
#endif
#if BLFM_ENABLED_SERVO4
  blfm_servomotor_apply(3, &cmd->servo4);
#endif
#endif


}
