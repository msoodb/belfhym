
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
#include "blfm_uart.h"

void blfm_board_init(void) {
  blfm_clock_init();
  blfm_gpio_init();

   // Enable DWT cycle counter for timing
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // Enable trace & debug
  }
  DWT->CYCCNT = 0;              // Reset counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // Enable cycle counter

  blfm_uart_init();
  blfm_i2c_init();
}
