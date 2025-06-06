
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_gpio.h"
#include "stm32f1xx.h"

void blfm_gpio_init(void) {
  // Enable GPIOA, GPIOB, and AFIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |
                RCC_APB2ENR_IOPBEN |
                RCC_APB2ENR_IOPCEN |
                RCC_APB2ENR_AFIOEN;

  // Optional: disable JTAG to free PB3, PB4, PB5 (retain SWD)
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0x3 << (pin * 4)); // Output, push-pull, 50 MHz (fastest)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
    gpio->CRH |= (0x3 << ((pin - 8) * 4)); // Output, push-pull, 50 MHz (fastest)
  }
}

void blfm_gpio_config_input(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin < 8) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0x4 << (pin * 4)); // Input floating
  } else {
    uint32_t shift = (pin - 8) * 4;
    gpio->CRH &= ~(0xF << shift);
    gpio->CRH |= (0x4 << ((pin - 8) * 4));
  }

  // Enable pull-up by default
  gpio->ODR |= (1 << pin);
}

void blfm_gpio_set_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BSRR = (1 << pin);
}

void blfm_gpio_clear_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BRR = (1 << pin);
}

int blfm_gpio_read_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  return (gpio->IDR & (1 << pin)) ? 1 : 0;
}

void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->ODR ^= (1 << pin);
}
