
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_adc.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"

void blfm_adc_init(void) {
  // Enable ADC1 and GPIOA clocks (pot on PA6)
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

  // Configure PA6 as analog input (clear MODE6 and CNF6 bits)
  GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);

  // ADC settings: Enable ADC
  ADC1->CR2 |= ADC_CR2_ADON;
  for (volatile int i = 0; i < 1000; i++);  // short delay

  // Start ADC calibration
  ADC1->CR2 |= ADC_CR2_CAL;
  while (ADC1->CR2 & ADC_CR2_CAL);  // wait for calibration complete
}

int blfm_adc_read(uint8_t channel, uint16_t *value) {
  if (!value || channel > 15) return -1;

  ADC1->SQR3 = channel;      // Select ADC channel
  ADC1->CR2 |= ADC_CR2_ADON; // Start conversion
  while (!(ADC1->SR & ADC_SR_EOC)); // Wait until conversion complete

  *value = (uint16_t)ADC1->DR;
  return 0;
}
