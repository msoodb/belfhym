/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_clock.h"
#include "stm32f1xx.h" /* IWYU pragma: keep */

/* Configure STM32F103 to run at 72MHz from external 8MHz crystal */
void blfm_clock_init(void) {
  /* External crystal starts running
   * MCU turns on the 8 MHz crystal (HSE) and waits until it's stable */
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY));

  /* Flash memory is made ready for higher speed
   * It adds extra "wait states" so Flash can keep up when CPU runs at 72 MHz */
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_2;

  /* PLL is configured to multiply the clock
   * It takes the 8 MHz crystal and multiplies it by 9 → 72 MHz */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
  RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

  /* PLL is turned on and locked
   * MCU waits until the PLL output is stable */
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));

  /* Bus speeds are set
   * CPU (AHB) → 72 MHz, APB1 (low-speed peripherals) → 36 MHz, APB2 (high-speed peripherals) → 72 MHz */
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;

  /* Switch main system clock to PLL
   * CPU now runs from the fast 72 MHz clock instead of the default internal 8 MHz */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  /* Enable AFIO (Alternate Function I/O)
   * Allows pin remapping and advanced GPIO features */
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  /* Tell the system its new speed
   * SystemCoreClockUpdate() updates the global SystemCoreClock variable so delays, timers, and libraries know the new CPU frequency */
  SystemCoreClockUpdate();
}
