/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_clock.h"
#include "stm32f4xx.h" /* IWYU pragma: keep */

void blfm_clock_init(void) {
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY));

  FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;

  RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE |
                  (25 << RCC_PLLCFGR_PLLM_Pos) |
                  (192 << RCC_PLLCFGR_PLLN_Pos) |
                  (0 << RCC_PLLCFGR_PLLP_Pos) |
                  (4 << RCC_PLLCFGR_PLLQ_Pos));

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));

  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  SystemCoreClockUpdate();

  SystemCoreClockUpdate();
}
