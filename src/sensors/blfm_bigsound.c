
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_bigsound.h"
#include "FreeRTOS.h"
#include "blfm_gpio.h"
#include "queue.h"
#include "stm32f1xx.h"
#include "task.h"

#define SOUND_PORT GPIOB
#define SOUND_PIN 7

#define SOUND_TASK_STACK_SIZE 128
#define SOUND_TASK_PRIORITY 2
#define SOUND_QUEUE_LENGTH 1

static QueueHandle_t sound_data_queue = NULL;
static TaskHandle_t sound_task_handle = NULL;

static void vSoundSensorTask(void *pvParameters);

void blfm_bigsound_init(void) {
  blfm_gpio_config_input((uint32_t)SOUND_PORT, SOUND_PIN);

  if (sound_data_queue == NULL) {
    sound_data_queue =
        xQueueCreate(SOUND_QUEUE_LENGTH, sizeof(blfm_bigsound_data_t));
    configASSERT(sound_data_queue != NULL);
  }

  if (sound_task_handle == NULL) {
    BaseType_t result =
        xTaskCreate(vSoundSensorTask, "SoundSensorTask", SOUND_TASK_STACK_SIZE,
                    NULL, SOUND_TASK_PRIORITY, &sound_task_handle);
    configASSERT(result == pdPASS);
    if (result == pdPASS) {
    }
  }
}

bool blfm_bigsound_read(blfm_bigsound_data_t *data) {
  if (!data || !sound_data_queue)
    return false;

  // Notify the task to check the sensor
  xTaskNotifyGive(sound_task_handle);

  // Wait for the data (with timeout)
  if (xQueueReceive(sound_data_queue, data, pdMS_TO_TICKS(50)) == pdPASS) {
    return true;
  }
  return false;
}

static bool blfm_sound_action(blfm_bigsound_data_t *data) {
  if (!data)
    return false;

  // Read the digital pin
  data->sound_detected = blfm_gpio_read_pin((uint32_t)SOUND_PORT, SOUND_PIN);
  return true;
}

static void vSoundSensorTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // Wait until someone calls read()
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    blfm_bigsound_data_t data;
    if (blfm_sound_action(&data)) {
      xQueueOverwrite(sound_data_queue, &data);
    }
  }
}
