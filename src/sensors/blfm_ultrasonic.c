
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_ultrasonic.h"
#include "FreeRTOS.h"
#include "blfm_gpio.h"
#include "queue.h"
#include "stm32f1xx.h"
#include "task.h"

#define TRIG_PORT GPIOB_BASE
#define TRIG_PIN 6
#define ECHO_PORT GPIOB_BASE
#define ECHO_PIN 3

#define ULTRASONIC_TASK_STACK_SIZE 256
#define ULTRASONIC_TASK_PRIORITY 2
#define ULTRASONIC_QUEUE_LENGTH 1

static QueueHandle_t ultrasonic_data_queue = NULL;
static TaskHandle_t ultrasonic_task_handle = NULL;

static void delay_us(uint32_t us) {
  for (uint32_t i = 0; i < us * 8; ++i) {
    __asm volatile("nop");
  }
}

static void vUltrasonicTask(void *pvParameters);

void blfm_ultrasonic_init(void) {
  blfm_gpio_config_output(TRIG_PORT, TRIG_PIN);
  blfm_gpio_config_input(ECHO_PORT, ECHO_PIN);
  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);

  if (ultrasonic_data_queue == NULL) {
    ultrasonic_data_queue =
        xQueueCreate(ULTRASONIC_QUEUE_LENGTH, sizeof(blfm_ultrasonic_data_t));
    configASSERT(ultrasonic_data_queue != NULL);
  }

  if (ultrasonic_task_handle == NULL) {
    BaseType_t result = xTaskCreate(
        vUltrasonicTask, "UltrasonicTask", ULTRASONIC_TASK_STACK_SIZE, NULL,
        ULTRASONIC_TASK_PRIORITY, &ultrasonic_task_handle);
    configASSERT(result == pdPASS);
  }
}

bool blfm_ultrasonic_read(blfm_ultrasonic_data_t *data) {
  if (!data || !ultrasonic_data_queue)
    return false;

  // Wake the task to take a fresh reading
  xTaskNotifyGive(ultrasonic_task_handle);

  // Wait for the new data (with timeout)
  if (xQueueReceive(ultrasonic_data_queue, data, pdMS_TO_TICKS(50)) == pdPASS) {
    return true;
  }
  return false;
  
}

static bool blfm_ultrasonic_action(blfm_ultrasonic_data_t *data) {
  if (!data)
    return false;

  data->distance_mm = 1000;
  return true;

  /*uint32_t start, end, duration;

  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);
  delay_us(2);
  blfm_gpio_set_pin(TRIG_PORT, TRIG_PIN);
  delay_us(10);
  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);

  while (!blfm_gpio_read_pin(ECHO_PORT, ECHO_PIN))
    ;

  start = DWT->CYCCNT;

  while (blfm_gpio_read_pin(ECHO_PORT, ECHO_PIN))
    ;

  end = DWT->CYCCNT;

  duration = end - start;
  uint32_t us = duration / (SystemCoreClock / 1000000);
  data->distance_mm = (uint16_t)(us / 58);

  return true;*/
}

static void vUltrasonicTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // Wait until someone calls read()
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    blfm_ultrasonic_data_t data;
    if (blfm_ultrasonic_action(&data)) {
      xQueueOverwrite(ultrasonic_data_queue, &data);
    }
  }
}
