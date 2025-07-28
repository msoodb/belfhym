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
#include "FreeRTOS.h"
#include "task.h"

#define SERVO_MAX_SERVOS 4

// SG90 servo: 180° total range (-90° to +90°) - based on working test
#define SERVO_MIN_ANGLE -90
#define SERVO_MAX_ANGLE 90
#define SERVO_CENTER_ANGLE 0

typedef struct {
  blfm_servo_type_t type;
  bool reverse_direction;
} servo_state_t;

static servo_state_t servo_states[SERVO_MAX_SERVOS];

static uint16_t angle_to_pulse_us(int8_t angle) {
  if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
  if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
  // Map -90° to +90° range to 500μs to 2500μs range - based on working test
  return 1500 + ((int16_t)angle * 1000) / 90;
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
  // Map proportional input (-1000 to +1000) to servo angle (-90° to +90°)
  int8_t angle = (cmd->proportional_input * 90) / 1000;
  int8_t final_angle = apply_direction_reverse(servo_id, angle);
  uint16_t pulse_us = angle_to_pulse_us(final_angle);
  blfm_pwm_set_pulse_us(servo_id, pulse_us);
}

// Simple delay function for PWM timing
static void simple_delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i++) {
    for (volatile uint32_t j = 0; j < 8000; j++) {
      // Simple busy wait - approximately 1ms at 72MHz
    }
  }
}

// Servo PWM task - continuously generates PWM cycles like working test
static void servo_pwm_task(void *pvParameters) {
  (void)pvParameters;
  
  while (1) {
    // Generate PWM pulse for all enabled channels
    blfm_pwm_generate_cycle();
    
    // Wait remaining time to complete 20ms period
    // After longest pulse (2500us), wait ~17.5ms more
    simple_delay_ms(17);
    
    // Give time for other tasks
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void blfm_servomotor_init(void) {
  blfm_pwm_init();
  
  // Initialize all servo states
  for (uint8_t i = 0; i < SERVO_MAX_SERVOS; i++) {
    servo_states[i].type = BLFM_SERVO_TYPE_MANUAL;
    servo_states[i].reverse_direction = false;
  }
  
  // Create servo PWM task
  xTaskCreate(servo_pwm_task, "ServoPWM", 128, NULL, 3, NULL);
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