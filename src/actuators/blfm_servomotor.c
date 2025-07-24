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

#define SERVO_TASK_STACK_SIZE 256
#define SERVO_TASK_PRIORITY 2
#define SERVO_UPDATE_MS 8

// SG90 servo: 90° total range (-45° to +45°)
#define SERVO_MIN_ANGLE -45
#define SERVO_MAX_ANGLE 45
#define SERVO_CENTER_ANGLE 0

typedef struct {
  blfm_servo_type_t type;
  bool reverse_direction;
  int8_t scan_min_angle;
  int8_t scan_max_angle;
} servo_state_t;

static TaskHandle_t servo_task_handle = NULL;
static servo_state_t servo_state = {
  .type = BLFM_SERVO_TYPE_MANUAL,
  .reverse_direction = false,
  .scan_min_angle = SERVO_MIN_ANGLE,
  .scan_max_angle = SERVO_MAX_ANGLE
};

static uint16_t angle_to_pulse_us(int8_t angle) {
  if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
  if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
  return 1500 + ((int16_t)angle * 1000) / 90;
}

static int8_t apply_direction_reverse(int8_t angle) {
  return servo_state.reverse_direction ? (-angle) : angle;
}

static void vServoTask(void *pvParameters) {
  (void)pvParameters;
  
  blfm_pwm_set_pulse_us(1500);  // Start at center
  
  for (;;) {
    if (servo_state.type == BLFM_SERVO_TYPE_SCANNER) {
      static uint32_t scan_counter = 0;
      static bool scan_direction = true;  // true = go to max, false = go to min
      
      scan_counter++;
      if (scan_counter >= 375) {  // Change direction every 3 seconds
        scan_counter = 0;
        scan_direction = !scan_direction;
        
        int8_t target_angle = scan_direction ? 
                              servo_state.scan_max_angle : 
                              servo_state.scan_min_angle;
        
        int8_t final_angle = apply_direction_reverse(target_angle);
        uint16_t pulse = angle_to_pulse_us(final_angle);
        blfm_pwm_set_pulse_us(pulse);
      }
    } else if (servo_state.type == BLFM_SERVO_TYPE_RADAR) {
      static uint32_t radar_counter = 0;
      static bool radar_direction = true;  // true = go to right, false = go to left
      
      radar_counter++;
      if (radar_counter >= 125) {  // Change direction every 1 second
        radar_counter = 0;
        radar_direction = !radar_direction;
        
        int8_t target_angle = radar_direction ? SERVO_MAX_ANGLE : SERVO_MIN_ANGLE;
        int8_t final_angle = apply_direction_reverse(target_angle);
        uint16_t pulse = angle_to_pulse_us(final_angle);
        blfm_pwm_set_pulse_us(pulse);
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(SERVO_UPDATE_MS));
  }
}

void blfm_servomotor_init(void) {
  if (servo_task_handle == NULL) {
    BaseType_t res = xTaskCreate(vServoTask, "ServoTask", SERVO_TASK_STACK_SIZE,
                                 NULL, SERVO_TASK_PRIORITY, &servo_task_handle);
    configASSERT(res == pdPASS);
    (void)res; // Suppress unused warning in release builds
  }
}

void blfm_servomotor_set_type(blfm_servo_type_t type) {
  servo_state.type = type;
}

void blfm_servomotor_apply(const blfm_servomotor_command_t *cmd) {
  if (!cmd) return;

  servo_state.reverse_direction = cmd->reverse_direction;

  switch (servo_state.type) {
    case BLFM_SERVO_TYPE_SCANNER:
      if (cmd->scan_min_angle <= SERVO_MAX_ANGLE) {
        servo_state.scan_min_angle = (int8_t)cmd->scan_min_angle;
      }
      if (cmd->scan_max_angle <= SERVO_MAX_ANGLE) {
        servo_state.scan_max_angle = (int8_t)cmd->scan_max_angle;
      }
      break;

    case BLFM_SERVO_TYPE_RADAR:
      // No parameters needed - uses full range automatically
      break;

    case BLFM_SERVO_TYPE_TRACKER:
      if (cmd->target_x != 0) {
        int16_t x = cmd->target_x;
        if (x < -1000) x = -1000;
        if (x > 1000) x = 1000;
        
        int8_t angle = (int8_t)((x * 45) / 1000);
        int8_t final_angle = apply_direction_reverse(angle);
        uint16_t pulse = angle_to_pulse_us(final_angle);
        blfm_pwm_set_pulse_us(pulse);
      }
      break;

    case BLFM_SERVO_TYPE_MANUAL:
      if (cmd->pulse_width_us >= 1000 && cmd->pulse_width_us <= 2000) {
        blfm_pwm_set_pulse_us(cmd->pulse_width_us);
      } else {
        int8_t angle = (int8_t)(cmd->angle - 90);
        if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
        if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
        
        int8_t final_angle = apply_direction_reverse(angle);
        uint16_t pulse = angle_to_pulse_us(final_angle);
        blfm_pwm_set_pulse_us(pulse);
      }
      break;

    case BLFM_SERVO_TYPE_STATIC:
      {
        static int8_t last_angle = SERVO_CENTER_ANGLE;
        int8_t angle = (int8_t)(cmd->angle - 90);
        if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
        if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
        
        if (angle != last_angle) {
          int8_t final_angle = apply_direction_reverse(angle);
          uint16_t pulse = angle_to_pulse_us(final_angle);
          blfm_pwm_set_pulse_us(pulse);
          last_angle = angle;
        }
      }
      break;

    case BLFM_SERVO_TYPE_PROPORTIONAL:
      {
        int16_t input = cmd->proportional_input;
        uint8_t deadband = (cmd->deadband > 0) ? cmd->deadband : 10;
        uint8_t travel_limit = (cmd->travel_limit > 0) ? cmd->travel_limit : 100;
        
        if (input > -deadband && input < deadband) {
          input = 0;
        }
        
        if (input < -1000) input = -1000;
        if (input > 1000) input = 1000;
        
        int16_t limited_range = (1000 * travel_limit) / 100;
        if (input > limited_range) input = limited_range;
        if (input < -limited_range) input = -limited_range;
        
        int8_t max_travel = (45 * travel_limit) / 100;
        int8_t angle = (input * max_travel) / limited_range;
        
        if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
        if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
        
        int8_t final_angle = apply_direction_reverse(angle);
        uint16_t pulse = angle_to_pulse_us(final_angle);
        blfm_pwm_set_pulse_us(pulse);
      }
      break;

    default:
      break;
  }
}

#endif /* BLFM_ENABLED_SERVO */