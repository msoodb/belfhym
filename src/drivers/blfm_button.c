
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_button.h"
#include "blfm_gpio.h"

void blfm_button_init(uint32_t port, uint32_t pin) {
  blfm_gpio_config_input(port, pin);
}

int blfm_button_is_pressed(uint32_t port, uint32_t pin) {
  return blfm_gpio_read_pin(port, pin) == 0; // Assuming active-low
}
