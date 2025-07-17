
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_uart.h"
#include "stm32f1xx.h"

void blfm_uart_init(void) {
  // Enable GPIOA and USART1
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

  // Configure PA9 (TX) as Alternate Function Push-Pull, 50 MHz
  GPIOA->CRH &= ~(0xF << 4);       // Clear PA9 bits
  GPIOA->CRH |=  (0xB << 4);       // CNF=10 (AF Push-Pull), MODE=11 (50 MHz)

  // Set baud rate for 72 MHz system clock and 115200 bps: 72e6 / 115200 ≈ 625
  USART1->BRR = 625;

  // Enable TX and USART
  USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
}

void blfm_uart_send_u8(uint8_t val) {
  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = val;
}

void blfm_uart_send_u32(uint32_t val) {
  blfm_uart_send_u8((val >> 24) & 0xFF);
  blfm_uart_send_u8((val >> 16) & 0xFF);
  blfm_uart_send_u8((val >> 8) & 0xFF);
  blfm_uart_send_u8(val & 0xFF);
}

