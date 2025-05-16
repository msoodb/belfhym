
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_gpio.h"

void blfm_gpio_config_output(uint32_t port, uint32_t pin) {
  (void) port;
  (void) pin;
  // TODO: configure pin as output
}

void blfm_gpio_config_input(uint32_t port, uint32_t pin) {
  (void) port;
  (void) pin;
  // TODO: configure pin as input
}

void blfm_gpio_set_pin(uint32_t port, uint32_t pin) {
  (void) port;
  (void) pin;
  // TODO: set GPIO pin high
}

void blfm_gpio_clear_pin(uint32_t port, uint32_t pin) {
  (void) port;
  (void) pin;
  // TODO: set GPIO pin low
}

int blfm_gpio_read_pin(uint32_t port, uint32_t pin) {
  (void) port;
  (void) pin;
  // TODO: return pin level (0 or 1)
  return 0;
}

