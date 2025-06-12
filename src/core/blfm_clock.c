
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_clock.h"
#include "stm32f1xx.h"

void blfm_clock_init(void) {
  // Enable HSE (external 8 MHz crystal)
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // Set flash latency (2 wait states)
  FLASH->ACR |= FLASH_ACR_LATENCY_2;

  // Configure PLL: 8 MHz HSE * 9 = 72 MHz
  RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  // Set AHB = SYSCLK, APB1 = SYSCLK/2, APB2 = SYSCLK
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;

  // Switch system clock to PLL
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;

  // Update CMSIS SystemCoreClock variable
  SystemCoreClockUpdate();
}
