
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

void blfm_gpio_config_output(uint32_t port, uint32_t pin);
void blfm_gpio_config_input(uint32_t port, uint32_t pin);
void blfm_gpio_set_pin(uint32_t port, uint32_t pin);
void blfm_gpio_clear_pin(uint32_t port, uint32_t pin);
int blfm_gpio_read_pin(uint32_t port, uint32_t pin);

#endif // BLFM_GPIO_H

