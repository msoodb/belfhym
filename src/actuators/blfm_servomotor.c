/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"
#if BLFM_ENABLED_SERVO

#include "blfm_servomotor.h"
#include "blfm_pwm.h"

#define SERVO_MAX_SERVOS 4

// SG90 servo: 90° total range (-45° to +45°)
#define SERVO_MIN_ANGLE -45
#define SERVO_MAX_ANGLE 45
#define SERVO_CENTER_ANGLE 0

typedef struct {
  blfm_servo_type_t type;
  bool reverse_direction;
} servo_state_t;

static servo_state_t servo_states[SERVO_MAX_SERVOS];

static uint16_t angle_to_pulse_us(int8_t angle) {
  if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
  if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
  // Map -45° to +45° range to 500μs to 2500μs range for SG90
  return 1500 + ((int16_t)angle * 2000) / 90;
}

static int8_t apply_direction_reverse(uint8_t servo_id, int8_t angle) {
  return servo_states[servo_id].reverse_direction ? (-angle) : angle;
}

static void process_manual_servo(uint8_t servo_id, const blfm_servomotor_command_t *cmd) {
  int8_t final_angle = apply_direction_reverse(servo_id, cmd->angle);
  uint16_t pulse_us = angle_to_pulse_us(final_angle);
  blfm_pwm_set_pulse_us(servo_id, pulse_us);
}

static void process_proportional_servo(uint8_t servo_id, const blfm_servomotor_command_t *cmd) {
  // Map proportional input (-1000 to +1000) to servo angle (-45° to +45°)
  int8_t angle = (cmd->proportional_input * 45) / 1000;
  int8_t final_angle = apply_direction_reverse(servo_id, angle);
  uint16_t pulse_us = angle_to_pulse_us(final_angle);
  blfm_pwm_set_pulse_us(servo_id, pulse_us);
}

void blfm_servomotor_init(void) {
  blfm_pwm_init();
  
  // Initialize all servo states
  for (uint8_t i = 0; i < SERVO_MAX_SERVOS; i++) {
    servo_states[i].type = BLFM_SERVO_TYPE_MANUAL;
    servo_states[i].reverse_direction = false;
  }
}

void blfm_servomotor_set_type(uint8_t servo_id, blfm_servo_type_t type) {
  if (servo_id >= SERVO_MAX_SERVOS) return;
  servo_states[servo_id].type = type;
}

void blfm_servomotor_apply(uint8_t servo_id, const blfm_servomotor_command_t *cmd) {
  if (!cmd || servo_id >= SERVO_MAX_SERVOS) return;
  
  switch (servo_states[servo_id].type) {
    case BLFM_SERVO_TYPE_MANUAL:
      process_manual_servo(servo_id, cmd);
      break;
    case BLFM_SERVO_TYPE_PROPORTIONAL:
      process_proportional_servo(servo_id, cmd);
      break;
    case BLFM_SERVO_TYPE_STATIC:
      // Static servo - do nothing
      break;
    default:
      break;
  }
}

#endif /* BLFM_ENABLED_SERVO */