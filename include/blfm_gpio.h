
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_GPIO_H
#define BLFM_GPIO_H

#include <stdint.h>
#include "stm32f1xx.h"

// Initialize GPIO peripheral clocks and optional remapping
void blfm_gpio_init(void);

// Configure given pin as output push-pull (2 MHz default)
void blfm_gpio_config_output(uint32_t port, uint32_t pin);

// Configure given pin as input with pull-up
void blfm_gpio_config_input(uint32_t port, uint32_t pin);

// Set (drive high) the given pin
void blfm_gpio_set_pin(uint32_t port, uint32_t pin);

// Clear (drive low) the given pin
void blfm_gpio_clear_pin(uint32_t port, uint32_t pin);

// Read the current state of the pin (returns 0 or 1)
int blfm_gpio_read_pin(uint32_t port, uint32_t pin);

// Toggle (drive high) the given pin
void blfm_gpio_toggle_pin(uint32_t port, uint32_t pin);

// Configure given pin as output analoge (2 MHz default)
void blfm_gpio_config_analog(uint32_t port, uint32_t pin);

#endif // BLFM_GPIO_H

