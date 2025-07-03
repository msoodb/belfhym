
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
  // Enable GPIOA, GPIOB, GPIOC and AFIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

  // Optional: disable JTAG to free PB3, PB4, PB5 (retain SWD)
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0x3 << (pin * 4));  // MODE=11 (50MHz), CNF=00 (Output Push-Pull)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
    gpio->CRH |= (0x3 << ((pin - 8) * 4));
  }
}

void blfm_gpio_config_input(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0x4 << (pin * 4));  // MODE=00, CNF=01 (Floating Input)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
    gpio->CRH |= (0x4 << ((pin - 8) * 4));
  }

  // Optional pull-up by setting ODR
  gpio->ODR |= (1 << pin);
}

void blfm_gpio_config_input_pullup(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0x8 << (pin * 4));  // MODE=00, CNF=10 (Input with Pull-up/down)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
    gpio->CRH |= (0x8 << ((pin - 8) * 4));
  }

  gpio->ODR |= (1 << pin);  // Pull-up
}

void blfm_gpio_config_analog(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));  // MODE=00, CNF=00 (Analog)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
  }
}

void blfm_gpio_config_alternate_pushpull(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7) {
    gpio->CRL &= ~(0xF << (pin * 4));
    gpio->CRL |= (0xB << (pin * 4));  // MODE=11 (50MHz), CNF=10 (AF Push-Pull)
  } else {
    gpio->CRH &= ~(0xF << ((pin - 8) * 4));
    gpio->CRH |= (0xB << ((pin - 8) * 4));
  }
}

void blfm_gpio_set_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BSRR = (1 << pin);
}

void blfm_gpio_clear_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BRR = (1 << pin);
}

void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->ODR ^= (1 << pin);
}

int blfm_gpio_read_pin(uint32_t port, uint32_t pin) {
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  return (gpio->IDR & (1 << pin)) ? 1 : 0;
}
