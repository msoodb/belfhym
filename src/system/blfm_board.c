
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
#include "blfm_i2c1.h"
#include "blfm_uart.h"
#include "blfm_adc.h"
#include "blfm_delay.h"
#include "blfm_pwm.h"

void blfm_board_init(void) {
  blfm_clock_init();    // System clocks
  blfm_gpio_init();     // All GPIO modes

  // Peripheral inits
  blfm_pwm_init();
  blfm_uart_init();
  blfm_i2c1_init();
  blfm_adc_init();

  // Enable cycle counter
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  blfm_delay_init();
}
