
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_led.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"

// Onboard LED: PC13
#define LED_ONBOARD_PORT GPIOC
#define LED_ONBOARD_PIN 13

// External LED: PB5
#define LED_EXTERNAL_PORT GPIOB
#define LED_EXTERNAL_PIN 5

void blfm_led_init(void) {
  //blfm_gpio_config_output((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
  blfm_gpio_config_output((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

void blfm_led_onboard_toggle(void) {
  blfm_gpio_toggle_pin((uint32_t)LED_ONBOARD_PORT, LED_ONBOARD_PIN);
}

void blfm_led_external_toggle(void) {
  blfm_gpio_toggle_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

void blfm_led_external_on(void) {
  blfm_gpio_set_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}

void blfm_led_external_off(void) {
  blfm_gpio_clear_pin((uint32_t)LED_EXTERNAL_PORT, LED_EXTERNAL_PIN);
}
