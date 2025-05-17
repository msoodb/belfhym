
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_actuator_hub.h"
#include "FreeRTOS.h"
#include "blfm_alarm.h"
#include "blfm_motor.h"
#include "semphr.h"

static QueueHandle_t motor_cmd_queue;

void blfm_actuator_hub_init(void) {
  motor_cmd_queue = xQueueCreate(4, sizeof(blfm_motor_command_t));
  blfm_motor_init();
  blfm_alarm_init();
}

void blfm_actuator_hub_start(void) {
  // Start actuator-related operations (mock)
}

void blfm_actuator_hub_update(void) {
  // Placeholder: Update/control actuators centrally
  // e.g. control motor speed, alarm state
}

void blfm_actuator_hub_task(void *params) {
  (void)params;
  blfm_motor_command_t cmd;
  for (;;) {
    if (xQueueReceive(motor_cmd_queue, &cmd, portMAX_DELAY) == pdTRUE) {
      blfm_motor_set_speed(cmd.speed, cmd.speed); // left/right same for now
    }
  }
}

void blfm_actuator_hub_send_motor_command(blfm_motor_command_t *cmd) {
  xQueueSend(motor_cmd_queue, cmd, 0);
}
