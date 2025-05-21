
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_led.h"
#include "stm32f1xx.h"

void blfm_led_init(void) {
  // Enable GPIOC and GPIOB clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;

  // --- On-board LED: PC13 ---
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
  GPIOC->CRH |= (0b10 << GPIO_CRH_MODE13_Pos);  // Output mode, 2MHz, push-pull

  // --- External LED: PB5 ---
  GPIOB->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
  GPIOB->CRL |= (0b10 << GPIO_CRL_MODE5_Pos);   // Output mode, 2MHz, push-pull
}

void blfm_led_onboard_toggle(void) {
  GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void blfm_led_external_toggle(void) {
  GPIOB->ODR ^= GPIO_ODR_ODR5;
}

void blfm_led_external_on(void) {
  GPIOB->BSRR = GPIO_BSRR_BS5;
}

void blfm_led_external_off(void) {
  GPIOB->BSRR = GPIO_BSRR_BR5;
}
