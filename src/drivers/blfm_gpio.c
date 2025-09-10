/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_gpio.h"

void blfm_gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0x3U << (pin * 4));
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0x3U << ((pin - 8U) * 4));
  }
}

void blfm_gpio_config_input_pullup(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0x8U << (pin * 4));
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0x8U << ((pin - 8U) * 4));
  }

  gpio->ODR |= (1U << pin);
}

void blfm_gpio_config_analog(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
  }
}

void blfm_gpio_config_peripheral(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0xBU << (pin * 4));
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0xBU << ((pin - 8U) * 4));
  }
}

void blfm_gpio_set_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BSRR = (1U << pin);
}

void blfm_gpio_clear_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BRR = (1U << pin);
}

void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->ODR ^= (1U << pin);
}

int blfm_gpio_read_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return 0;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  return (gpio->IDR & (1U << pin)) ? 1 : 0;
}
