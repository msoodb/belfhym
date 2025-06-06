
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_delay.h"
#include "stm32f1xx.h" // or your MCU-specific header

static uint32_t ticks_per_us;

void blfm_delay_init(void) {
  // SysTick clock frequency = SystemCoreClock (Hz)
  // Calculate ticks per microsecond
  ticks_per_us = SystemCoreClock / 1000000;

  // Configure SysTick for simple countdown (no interrupt)
  SysTick->LOAD = 0xFFFFFF; // Max reload value
  SysTick->VAL = 0;         // Clear current value
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
}

void blfm_delay_us(uint32_t us) {
  uint32_t start_tick = SysTick->VAL;
  uint32_t ticks = us * ticks_per_us;

  // SysTick counts down, so compute target count
  while ((start_tick - SysTick->VAL) < ticks) {
    // wait
  }
}

void blfm_delay_ms(uint32_t ms) {
  while (ms--) {
    blfm_delay_us(1000);
  }
}
