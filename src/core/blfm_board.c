
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_board.h"
#include "blfm_clock.h"
#include "blfm_gpio.h"
#include "blfm_i2c.h"

void blfm_board_init(void) {
  //blfm_clock_init();
  blfm_gpio_init();
  //blfm_uart_init();
  //blfm_i2c_init();
}
