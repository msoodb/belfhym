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
#include "blfm_pins.h"
#include "stm32f4xx.h"

//==============================================================================
// Private functions
//==============================================================================

static void led_on(void) {
  blfm_gpio_clear_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);
}

static void led_off(void) {
  blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);
}

static void delay_ms(uint32_t ms) {
  uint32_t cycles = ms * (SystemCoreClock / 1000);
  uint32_t start = DWT->CYCCNT;
  while ((DWT->CYCCNT - start) < cycles);
}

static void blink_once(uint32_t duration_ms) {
  led_on();
  delay_ms(duration_ms);
  led_off();
}

//==============================================================================
// Public API - Simple and effective
//==============================================================================

void blfm_debug_signal_init(void) {
  // 1 long blink = INIT
  blink_once(500);
}

void blfm_debug_signal_success(void) {
  // 2 quick blinks = SUCCESS
  blink_once(150);
  delay_ms(150);
  blink_once(150);
}

void blfm_debug_signal_error(void) {
  // 5 quick blinks = ERROR
  for (uint8_t i = 0; i < 5; i++) {
    blink_once(100);
    if (i < 4) delay_ms(100);
  }
}

void blfm_debug_flash_quick(uint8_t n) {
  // N quick flashes
  for (uint8_t i = 0; i < n; i++) {
    blink_once(100);
    if (i < n - 1) delay_ms(100);
  }
}

void blfm_debug_blink_byte(uint8_t value) {
  // Show byte value as blink pattern (MSB first)
  for (int8_t bit = 7; bit >= 0; bit--) {
    if (value & (1 << bit)) {
      blink_once(400); // Long blink = 1
    } else {
      blink_once(100); // Short blink = 0  
    }
    delay_ms(150); // Gap between bits
  }
  delay_ms(800); // Longer pause after byte
}
