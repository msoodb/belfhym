/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Homa.
 */

#ifndef BLFM_GPIO_H
#define BLFM_GPIO_H

#include "stm32f4xx.h" /* IWYU pragma: keep */

void blfm_gpio_init(void);

void blfm_gpio_config_output(uint32_t port, uint32_t pin);
void blfm_gpio_config_input_pullup(uint32_t port, uint32_t pin);
void blfm_gpio_config_analog(uint32_t port, uint32_t pin);
void blfm_gpio_config_peripheral(uint32_t port, uint32_t pin, uint32_t af);

void blfm_gpio_set_pin(uint32_t port, uint32_t pin);
void blfm_gpio_clear_pin(uint32_t port, uint32_t pin);
void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin);
int  blfm_gpio_read_pin(uint32_t port, uint32_t pin);

#endif /* BLFM_GPIO_H */
