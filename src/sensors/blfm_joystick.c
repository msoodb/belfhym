
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_joystick.h"
#include "FreeRTOS.h"
#include "blfm_adc.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

static QueueHandle_t joystick_data_queue = NULL;
static TaskHandle_t joystick_task_handle = NULL;

static void vJoystickTask(void *pvParameters);

void blfm_joystick_init(void) {
  blfm_gpio_config_analog((uint32_t)BLFM_JOYSTICK_X_PORT, BLFM_JOYSTICK_X_PIN);
  blfm_gpio_config_analog((uint32_t)BLFM_JOYSTICK_Y_PORT, BLFM_JOYSTICK_Y_PIN);
  blfm_gpio_config_input((uint32_t)BLFM_JOYSTICK_BUTTON_PORT,
                         BLFM_JOYSTICK_BUTTON_PIN);

  if (joystick_data_queue == NULL) {
    joystick_data_queue =
        xQueueCreate(JOYSTICK_QUEUE_LENGTH, sizeof(blfm_joystick_data_t));
    configASSERT(joystick_data_queue != NULL);
  }

  if (joystick_task_handle == NULL) {
    BaseType_t result =
        xTaskCreate(vJoystickTask, "JoystickTask", JOYSTICK_TASK_STACK_SIZE,
                    NULL, JOYSTICK_TASK_PRIORITY, &joystick_task_handle);
    configASSERT(result == pdPASS);
    if (result != pdPASS) {
    }
  }
}

bool blfm_joystick_read(blfm_joystick_data_t *data) {
  if (!data || !joystick_data_queue)
    return false;

  xTaskNotifyGive(joystick_task_handle);
  if (xQueueReceive(joystick_data_queue, data, pdMS_TO_TICKS(50)) == pdPASS) {
    return true;
  }

  return false;
}

static void vJoystickTask(void *pvParameters) {
  (void)pvParameters;

  const uint16_t center = 2048;
  const uint16_t deadzone = 500;

  for (;;) {
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));

    blfm_joystick_data_t data;
    if (blfm_adc_read(BLFM_JOYSTICK_X_PIN, &data.x) != 0)
      continue;
    if (blfm_adc_read(BLFM_JOYSTICK_Y_PIN, &data.y) != 0)
      continue;
    data.pressed =
        !(BLFM_JOYSTICK_BUTTON_PORT->IDR & (1 << BLFM_JOYSTICK_BUTTON_PIN));
    xQueueOverwrite(joystick_data_queue, &data);

    // Interpret direction
    blfm_joystick_event_t event = {0};
    event.timestamp = xTaskGetTickCount();
    event.event_type =
        data.pressed ? BLFM_JOYSTICK_EVENT_PRESSED : BLFM_JOYSTICK_EVENT_NONE;

    if (data.y > center + deadzone)
      event.direction = BLFM_JOYSTICK_DIR_UP;
    else if (data.y < center - deadzone)
      event.direction = BLFM_JOYSTICK_DIR_DOWN;
    else if (data.x > center + deadzone)
      event.direction = BLFM_JOYSTICK_DIR_RIGHT;
    else if (data.x < center - deadzone)
      event.direction = BLFM_JOYSTICK_DIR_LEFT;
    else
      event.direction = BLFM_JOYSTICK_DIR_NONE;
    xQueueOverwrite(joystick_data_queue, &event);
  }
}
