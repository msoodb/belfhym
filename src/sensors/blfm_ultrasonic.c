
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_ultrasonic.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"

#define TRIG_PORT GPIOA_BASE
#define TRIG_PIN 1
#define ECHO_PORT GPIOA_BASE
#define ECHO_PIN 2

static void delay_us(uint32_t us) {
  // Rough delay (depends on 72 MHz clock)
  for (uint32_t i = 0; i < us * 8; ++i) {
    __asm volatile("nop");
  }
}

void blfm_ultrasonic_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  blfm_gpio_config_output(TRIG_PORT, TRIG_PIN);
  blfm_gpio_config_input(ECHO_PORT, ECHO_PIN);

  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);
}

uint16_t blfm_ultrasonic_get_distance(void) {
  uint32_t start, end, duration;

  // Trigger pulse
  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);
  delay_us(2);
  blfm_gpio_set_pin(TRIG_PORT, TRIG_PIN);
  delay_us(10);
  blfm_gpio_clear_pin(TRIG_PORT, TRIG_PIN);

  // Wait for echo to go high
  while (!blfm_gpio_read_pin(ECHO_PORT, ECHO_PIN))
    ;

  start = DWT->CYCCNT;

  // Wait for echo to go low
  while (blfm_gpio_read_pin(ECHO_PORT, ECHO_PIN))
    ;

  end = DWT->CYCCNT;

  duration = end - start;
  uint32_t us =
      duration / (SystemCoreClock / 1000000); // Convert to microseconds
  return (uint16_t)(us / 58);                 // Convert to mm
}
