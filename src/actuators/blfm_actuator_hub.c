
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
#include "blfm_led.h"
#include "blfm_display.h"
#include "semphr.h"

#define BLFM_LED_TASK_STACK 128
#define BLFM_LED_TASK_PRIORITY 1

//static QueueHandle_t motor_cmd_queue;

void blfm_actuator_hub_init(void) {
  //motor_cmd_queue = xQueueCreate(4, sizeof(blfm_motor_command_t));
  blfm_motor_init();
  blfm_alarm_init();
  blfm_led_init();
  //blfm_display_init();
}

void blfm_actuator_hub_start(void) {
  //blfm_display_test_message();
  xTaskCreate(vActuatorLedTask, "ActuatorLED", BLFM_LED_TASK_STACK, NULL, BLFM_LED_TASK_PRIORITY, NULL);
}

void blfm_actuator_hub_update(void) {
  // Placeholder: Update/control actuators centrally
  // e.g. control motor speed, alarm state
}

static void vActuatorLedTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    blfm_led_onboard_toggle();
    blfm_led_external_toggle();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/*void blfm_actuator_hub_task(void *params) {
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
*/
