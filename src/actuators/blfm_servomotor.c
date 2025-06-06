
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
#include "blfm_delay.h"
#include "blfm_gpio.h"
#include "queue.h"
#include "task.h"
#include <stddef.h>

#define BLFM_SERVO_PIN 6
#define BLFM_SERVO_PORT GPIOB

#define SERVO_TASK_STACK_SIZE 128
#define SERVO_TASK_PRIORITY 1
#define SERVO_QUEUE_LENGTH 1

static QueueHandle_t servo_command_queue = NULL;
static TaskHandle_t servo_task_handle = NULL;

static void vServoTask(void *pvParameters);

void blfm_servomotor_init(void) {
  blfm_gpio_config_output((uint32_t)BLFM_SERVO_PORT, BLFM_SERVO_PIN);

  if (servo_command_queue == NULL) {
    servo_command_queue =
        xQueueCreate(SERVO_QUEUE_LENGTH, sizeof(blfm_servomotor_command_t));
    configASSERT(servo_command_queue != NULL);
  }

  if (servo_task_handle == NULL) {
    BaseType_t res = xTaskCreate(vServoTask, "ServoTask", SERVO_TASK_STACK_SIZE,
                                 NULL, SERVO_TASK_PRIORITY, &servo_task_handle);
    configASSERT(res == pdPASS);
    if (res != pdPASS) {}
  }
}

// In blfm_servomotor.c
void blfm_servomotor_apply(const blfm_servomotor_command_t *cmd) {
  if (!cmd || !servo_command_queue)
    return;
  xQueueOverwrite(servo_command_queue, cmd);
}

static void vServoTask(void *pvParameters) {
  (void)pvParameters;

  blfm_servomotor_command_t current_cmd = {0}; // default zero init

  for (;;) {
    blfm_servomotor_command_t new_cmd;

    // Non-blocking check for new command
    if (xQueueReceive(servo_command_queue, &new_cmd, 0) == pdPASS) {
      current_cmd = new_cmd;
    }

    // Calculate pulse width
    uint16_t pulse_width_us = 1500; // default center pulse 1.5ms
    if (current_cmd.pulse_width_us >= 1000 &&
        current_cmd.pulse_width_us <= 2000) {
      pulse_width_us = current_cmd.pulse_width_us;
    } else {
      uint8_t angle = current_cmd.angle;
      if (angle > 180)
        angle = 180;
      pulse_width_us = 1000 + ((uint32_t)angle * 1000) / 180;
    }

    // Generate servo pulse (blocking)
    blfm_gpio_set_pin((uint32_t)BLFM_SERVO_PORT, BLFM_SERVO_PIN);
    blfm_delay_us(pulse_width_us);
    blfm_gpio_clear_pin((uint32_t)BLFM_SERVO_PORT, BLFM_SERVO_PIN);
    blfm_delay_us(20000 - pulse_width_us);
  }
}
