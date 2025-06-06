
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_servomotor.h"
#include "FreeRTOS.h"
#include "blfm_pwm.h"
#include "queue.h"
#include "task.h"
#include <stddef.h>

#define SERVO_TASK_STACK_SIZE 256
#define SERVO_TASK_PRIORITY 2
#define SERVO_QUEUE_LENGTH 1

static QueueHandle_t servo_command_queue = NULL;
static TaskHandle_t servo_task_handle = NULL;

static uint16_t angle_to_pulse_us(uint8_t angle);
static void servo_smooth_move(uint16_t *current_pulse, uint16_t target_pulse);

static void vServoTask(void *pvParameters);

void blfm_servomotor_init(void) {
  if (servo_command_queue == NULL) {
    servo_command_queue =
        xQueueCreate(SERVO_QUEUE_LENGTH, sizeof(blfm_servomotor_command_t));
    configASSERT(servo_command_queue != NULL);
  }

  if (servo_task_handle == NULL) {
    BaseType_t res = xTaskCreate(vServoTask, "ServoTask", SERVO_TASK_STACK_SIZE,
                                 NULL, SERVO_TASK_PRIORITY, &servo_task_handle);
    configASSERT(res == pdPASS);
    if (res != pdPASS) {
    }
  }
}

void blfm_servomotor_apply(const blfm_servomotor_command_t *cmd) {
  if (!cmd || !servo_command_queue)
    return;
  xQueueOverwrite(servo_command_queue, cmd);
}

static uint16_t angle_to_pulse_us(uint8_t angle) {
  if (angle > 180)
    angle = 180;
  return 1000 + ((uint32_t)angle * 1000) / 180; // Map 0-180 to 1000-2000us
}

// Smoothly move servo from current to target pulse in steps
static void servo_smooth_move(uint16_t *current_pulse, uint16_t target_pulse) {
  if (*current_pulse == target_pulse)
    return;

  int16_t diff = (int16_t)target_pulse - (int16_t)(*current_pulse);

  // Step size: choose something reasonable (e.g. 10 us)
  int16_t step = 10;

  if (diff > 0) {
    if (diff < step)
      *current_pulse = target_pulse;
    else
      *current_pulse += step;
  } else {
    if (-diff < step)
      *current_pulse = target_pulse;
    else
      *current_pulse -= step;
  }

  blfm_pwm_set_pulse_us(*current_pulse);
  vTaskDelay(pdMS_TO_TICKS(20)); // Wait 20 ms per step (~50Hz)
}

static void vServoTask(void *pvParameters) {
  (void)pvParameters;

  uint16_t current_pulse = angle_to_pulse_us(90); // Start centered
  uint16_t target_pulse = current_pulse;

  for (;;) {
    blfm_servomotor_command_t new_cmd;

    // Try to get new command from queue (non-blocking)
    if (xQueueReceive(servo_command_queue, &new_cmd, 0) == pdPASS) {
      if (new_cmd.pulse_width_us >= 1000 && new_cmd.pulse_width_us <= 2000) {
        target_pulse = new_cmd.pulse_width_us;
      } else {
        target_pulse = angle_to_pulse_us(new_cmd.angle);
      }
    }

    // Smoothly move servo towards target pulse
    servo_smooth_move(&current_pulse, target_pulse);
  }
}
