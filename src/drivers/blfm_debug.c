
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_debug.h"
#include "stm32f1xx.h"

void blfm_debug_init(void) {}

void test_single_pin_on_port(void) {
  // ====== CONFIGURE HERE ======
  GPIO_TypeDef *PORT = GPIOA; // Change to GPIOA, GPIOB, or GPIOC
  int PIN = 3;                // Change to 0–15
  // ============================

  // Enable clock for the selected port
  if (PORT == GPIOA) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  } else if (PORT == GPIOB) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  } else if (PORT == GPIOC) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  }

  // Configure pin as output (2 MHz, push-pull)
  if (PIN < 8) {
    PORT->CRL &= ~(0b1111 << (PIN * 4));
    PORT->CRL |= (0b0010 << (PIN * 4)); // 0010: output 2MHz push-pull
  } else {
    PORT->CRH &= ~(0b1111 << ((PIN - 8) * 4));
    PORT->CRH |= (0b0010 << ((PIN - 8) * 4));
  }

  while (1) {
    // Set pin high
    PORT->BSRR = (1 << PIN);

    // Delay ~500 ms
    for (volatile int i = 0; i < 50000; i++)
      __asm__("nop");

    // Set pin low
    PORT->BRR = (1 << PIN);

    // Delay ~500 ms
    for (volatile int i = 0; i < 500000; i++)
      __asm__("nop");
  }
}

void test_all_pins_on_port(void) {
  // ====== CONFIGURE HERE ======
  GPIO_TypeDef *PORT = GPIOA; // Change to GPIOA, GPIOB, or GPIOC
  // ============================

  // Enable clock for the selected port
  if (PORT == GPIOA) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  } else if (PORT == GPIOB) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  } else if (PORT == GPIOC) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  }

  // Configure all pins as output (2 MHz, push-pull)
  for (int pin = 0; pin < 8; pin++) {
    PORT->CRL &= ~(0b1111 << (pin * 4));
    PORT->CRL |= (0b0010 << (pin * 4)); // 0010: output 2MHz push-pull
  }
  for (int pin = 8; pin < 16; pin++) {
    PORT->CRH &= ~(0b1111 << ((pin - 8) * 4));
    PORT->CRH |= (0b0010 << ((pin - 8) * 4));
  }

  while (1) {
    for (int pin = 0; pin < 16; pin++) {
      // Set pin high
      PORT->BSRR = (1 << pin);

      // Delay ~500 ms
      for (volatile int i = 0; i < 10000; i++)
        __asm__("nop");

      // Set pin low
      PORT->BRR = (1 << pin);

      // Short delay before next
      for (volatile int i = 0; i < 5000; i++)
        __asm__("nop");
    }
  }
}
