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
#include "stm32f1xx.h"
#include "task.h"

static volatile uint32_t blink_rate = 500;

void blfm_led_toggle(void) { GPIOC->ODR ^= GPIO_ODR_ODR13; }

void blfm_led_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
  GPIOC->CRH |= GPIO_CRH_MODE13_0; // Output, 10 MHz
}

void blfm_led_set_blink_rate(uint32_t rate_ms) { blink_rate = rate_ms; }

void blfm_led_blink_task(void *params) {

  (void) params;
  while (1) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    vTaskDelay(pdMS_TO_TICKS(blink_rate));
  }
}
