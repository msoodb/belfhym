
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_adc.h"
#include "stm32f1xx.h"

// Simple init: single ADC, software-triggered
void blfm_adc_init(void) {
  // Enable ADC and GPIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // if using PAx pins

  // Configure pin as analog input (you must match the pin you wire)
  // Example for PA0 (ADC Channel 0)
  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

  // ADC settings
  ADC1->CR2 |= ADC_CR2_ADON; // Enable ADC
  for (volatile int i = 0; i < 1000; i++)
    ; // Short delay

  ADC1->CR2 |= ADC_CR2_CAL; // Start calibration
  while (ADC1->CR2 & ADC_CR2_CAL)
    ; // Wait for calibration to finish
}

// Read a single ADC channel
int blfm_adc_read(uint8_t channel, uint16_t *value) {
  if (!value || channel > 15)
    return -1;

  ADC1->SQR3 = channel;      // Set channel
  ADC1->CR2 |= ADC_CR2_ADON; // Start conversion
  while (!(ADC1->SR & ADC_SR_EOC))
    ; // Wait until done

  *value = (uint16_t)ADC1->DR;
  return 0;
}
