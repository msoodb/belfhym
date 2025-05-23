
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
#include "stm32f1xx.h"
#include "task.h"

// Onboard LED: PC13
#define LED_ONBOARD_PORT GPIOC
#define LED_ONBOARD_PIN 13

// External LED: PB5
#define LED_EXTERNAL_PORT GPIOB
#define LED_EXTERNAL_PIN 5

// Internal state for blinking
#define LED_TASK_INTERVAL_MS 10

static blfm_led_command_t current_cmd = {0};
static TickType_t last_toggle_tick = 0;
static BaseType_t led_state = 0;

static void blfm_led_onboard_on(void);
static void blfm_led_onboard_off(void);
static void blfm_led_external_on(void);
static void blfm_led_external_off(void);
static void vLedTask(void *pvParameters);

void blfm_led_init(void) {
  blfm_gpio_config_output((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
  blfm_gpio_config_output((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);

  current_cmd.mode = BLFM_LED_MODE_BLINK;
  current_cmd.blink_speed_ms = 500;
 
  xTaskCreate(vLedTask, "LEDTask", 256, NULL, 1, NULL);
}

void blfm_led_apply(const blfm_led_command_t cmd) {
  current_cmd = cmd;
  last_toggle_tick = xTaskGetTickCount();
}

static void blfm_led_onboard_on(void) {
  blfm_gpio_clear_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

static void blfm_led_onboard_off(void) {
  blfm_gpio_set_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

static void blfm_led_external_on(void) {
  blfm_gpio_set_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

static void blfm_led_external_off(void) {
  blfm_gpio_clear_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

static void vLedTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (current_cmd.mode == BLFM_LED_MODE_BLINK) {
      TickType_t now = xTaskGetTickCount();
      if (now - last_toggle_tick >= pdMS_TO_TICKS(current_cmd.blink_speed_ms)) {
        led_state = !led_state;

        if (led_state) {
          blfm_led_onboard_on();
          blfm_led_external_on();
        } else {
          blfm_led_onboard_off();
          blfm_led_external_off();
        }

        last_toggle_tick = now;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(LED_TASK_INTERVAL_MS));
  }
}
