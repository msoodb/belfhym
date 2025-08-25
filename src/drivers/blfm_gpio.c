/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_gpio.h"

/* Enable GPIO clocks and disable JTAG to free pins for robot use */
void blfm_gpio_init(void) {
  /* Enable GPIOA, GPIOB, GPIOC clocks (AFIO already enabled in clock_init) */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

  /* Disable JTAG to free PB3, PB4, PB5 (retain SWD) */
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

/* Configure pin as digital output (50MHz push-pull) */
void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0x3U << (pin * 4));  /* MODE=11 (50MHz), CNF=00 (Output Push-Pull) */
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0x3U << ((pin - 8U) * 4));
  }
}

/* Configure pin as digital input with internal pull-up resistor */
void blfm_gpio_config_input_pullup(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0x8U << (pin * 4));  /* MODE=00, CNF=10 (Input with Pull-up/down) */
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0x8U << ((pin - 8U) * 4));
  }

  gpio->ODR |= (1U << pin);  /* Pull-up */
}

/* Configure pin as analog input for ADC readings */
void blfm_gpio_config_analog(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));  /* MODE=00, CNF=00 (Analog) */
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
  }
}

/* Configure pin for hardware peripheral control (UART, SPI, PWM, etc.) */
void blfm_gpio_config_peripheral(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;

  if (pin <= 7U) {
    gpio->CRL &= ~(0xFU << (pin * 4));
    gpio->CRL |= (0xBU << (pin * 4));  /* MODE=11 (50MHz), CNF=10 (AF Push-Pull) */
  } else {
    gpio->CRH &= ~(0xFU << ((pin - 8U) * 4));
    gpio->CRH |= (0xBU << ((pin - 8U) * 4));
  }
}

/* Set pin to logic high (3.3V) */
void blfm_gpio_set_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BSRR = (1U << pin);
}

/* Set pin to logic low (0V) */
void blfm_gpio_clear_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->BRR = (1U << pin);
}

/* Toggle pin state (high->low or low->high) */
void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  gpio->ODR ^= (1U << pin);
}

/* Read current pin state (returns 1 for high, 0 for low) */
int blfm_gpio_read_pin(uint32_t port, uint32_t pin) {
  if (pin > 15U) return 0;
  GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
  return (gpio->IDR & (1U << pin)) ? 1 : 0;
}
