
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_led.h"
#include "FreeRTOS.h"
#include "blfm_gpio.h"
#include "blfm_types.h"
#include "queue.h"
#include "stm32f1xx.h"
#include "task.h"

#define LED_TASK_STACK_SIZE 512
#define LED_TASK_PRIORITY 2
#define LED_QUEUE_LENGTH 5

// Onboard LED: PC13
#define LED_ONBOARD_PORT GPIOC
#define LED_ONBOARD_PIN 13

// External LED: PB5
#define LED_EXTERNAL_PORT GPIOB
#define LED_EXTERNAL_PIN 5

// Internal state for blinking
#define LED_TASK_INTERVAL_MS 10

static void blfm_led_onboard_on(void);
static void blfm_led_onboard_off(void);
static void blfm_led_onboard_toggle(void);
static void blfm_led_external_on(void);
static void blfm_led_external_off(void);
static void blfm_led_external_toggle(void);
static void vLedTask(void *pvParameters);

static QueueHandle_t led_command_queue = NULL;
static TaskHandle_t led_task_handle = NULL;

void blfm_led_init(void) {
  blfm_gpio_config_output((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
  blfm_gpio_config_output((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);

  if (led_command_queue == NULL) {
    led_command_queue =
        xQueueCreate(LED_QUEUE_LENGTH, sizeof(blfm_led_command_t));
    configASSERT(led_command_queue != NULL);
  }

  if (led_task_handle == NULL) {
    BaseType_t result = xTaskCreate(vLedTask, "LEDTask", LED_TASK_STACK_SIZE,
                                    NULL, LED_TASK_PRIORITY, &led_task_handle);
    configASSERT(result == pdPASS);
  }
}

void blfm_led_apply(const blfm_led_command_t *cmd) {
  if (!cmd || !led_command_queue)
    return;
  xQueueOverwrite(led_command_queue, cmd);
}

static void blfm_led_onboard_on(void) {
  blfm_gpio_clear_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

static void blfm_led_onboard_off(void) {
  blfm_gpio_set_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

static void blfm_led_onboard_toggle(void) {
  blfm_gpio_toggle_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

static void blfm_led_external_on(void) {
  blfm_gpio_set_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

static void blfm_led_external_off(void) {
  blfm_gpio_clear_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
 }

static void blfm_led_external_toggle(void) {
  blfm_gpio_toggle_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}
 
static void vLedTask(void *pvParameters) {
  (void)pvParameters;
  blfm_led_command_t current_cmd = {.mode = BLFM_LED_MODE_OFF,
                                    .blink_speed_ms = 50};
  for (;;) {
    blfm_led_command_t received_cmd;

    // Non-blocking receive, if a command available, update current_cmd
    if (xQueueReceive(led_command_queue, &received_cmd, 0) == pdPASS) {
      current_cmd = received_cmd;
    }

    switch (current_cmd.mode) {
    case BLFM_LED_MODE_OFF:
      blfm_led_onboard_off();
      blfm_led_external_off();
      break;

    case BLFM_LED_MODE_ON:
      blfm_led_onboard_on();
      blfm_led_external_on();
      break;

    case BLFM_LED_MODE_BLINK:
      blfm_led_onboard_toggle();
      blfm_led_external_toggle();
      break;
      
    default:
      // Unknown mode — turn off LED to be safe
      blfm_led_onboard_off();
      blfm_led_external_off();
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(current_cmd.blink_speed_ms)); // Delay 1000 ms
  }
}
