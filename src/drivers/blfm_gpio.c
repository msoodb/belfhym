/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Homa.
 */

#include "blfm_gpio.h"

void blfm_gpio_init(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  gpio->MODER &= ~(3U << (pin * 2));
  gpio->MODER |= (1U << (pin * 2));
  gpio->OTYPER &= ~(1U << pin);
  gpio->OSPEEDR |= (3U << (pin * 2));
  gpio->PUPDR &= ~(3U << (pin * 2));
}

void blfm_gpio_config_input_pullup(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  gpio->MODER &= ~(3U << (pin * 2));
  gpio->OTYPER &= ~(1U << pin);
  gpio->OSPEEDR |= (3U << (pin * 2));
  gpio->PUPDR &= ~(3U << (pin * 2));
  gpio->PUPDR |= (1U << (pin * 2));
}

void blfm_gpio_config_analog(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  gpio->MODER |= (3U << (pin * 2));
  gpio->OTYPER &= ~(1U << pin);
  gpio->PUPDR &= ~(3U << (pin * 2));
}

void blfm_gpio_config_peripheral(uint32_t port, uint32_t pin, uint32_t af) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  gpio->MODER &= ~(3U << (pin * 2));
  gpio->MODER |= (2U << (pin * 2));
  gpio->OTYPER &= ~(1U << pin);
  gpio->OSPEEDR |= (3U << (pin * 2));
  gpio->PUPDR &= ~(3U << (pin * 2));

  if (pin <= 7U) {
    gpio->AFR[0] &= ~(0xFU << (pin * 4));
    gpio->AFR[0] |= (af << (pin * 4));
  } else {
    gpio->AFR[1] &= ~(0xFU << ((pin - 8U) * 4));
    gpio->AFR[1] |= (af << ((pin - 8U) * 4));
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
  gpio->BSRR = (1U << (pin + 16));
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
