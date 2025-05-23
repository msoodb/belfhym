
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_debug.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"

// Onboard LED: PC13
#define DEBUG_LED_ONBOARD_PORT GPIOC
#define DEBUG_LED_ONBOARD_PIN 13

#define DEBUG_LED_EXTERNAL_PORT GPIOB
#define DEBUG_LED_EXTERNAL_PIN 5

void blfm_debug_init(void) {
  blfm_gpio_config_output((uint32_t)DEBUG_LED_ONBOARD_PORT, DEBUG_LED_ONBOARD_PIN);
  blfm_gpio_config_output((uint32_t)DEBUG_LED_EXTERNAL_PORT, DEBUG_LED_EXTERNAL_PIN);
}

void blfm_debug_onbard_on(void) { 
  blfm_gpio_clear_pin((uint32_t)DEBUG_LED_ONBOARD_PORT, DEBUG_LED_ONBOARD_PIN);
}

void blfm_debug_onbard_off(void) { 
  blfm_gpio_set_pin((uint32_t)DEBUG_LED_ONBOARD_PORT, DEBUG_LED_ONBOARD_PIN);
}

void blfm_debug_external_on(void) { 
  blfm_gpio_set_pin((uint32_t)DEBUG_LED_EXTERNAL_PORT, DEBUG_LED_EXTERNAL_PIN);
}

void blfm_debug_external_off(void) { 
  blfm_gpio_clear_pin((uint32_t)DEBUG_LED_EXTERNAL_PORT, DEBUG_LED_EXTERNAL_PIN);
}
