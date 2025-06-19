
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
#include "blfm_adc.h"
#include "blfm_delay.h"
#include "blfm_pwm.h"
#include "blfm_pins.h"

void blfm_board_init(void) {
  blfm_clock_init();
  blfm_gpio_init();
  blfm_pwm_init();
  
// Correct and early, but *after clocks are stable*
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  
  blfm_uart_init();
  blfm_i2c_init();
  blfm_adc_init();
  blfm_delay_init();
}
