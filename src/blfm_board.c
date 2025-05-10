/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_board.h"
#include "stm32f1xx.h"

void blfm_board_led_init(void) {
    // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Set PC13 as general-purpose output push-pull, 10 MHz
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= (GPIO_CRH_MODE13_0);
}

void blfm_board_led_toggle(void) {
    GPIOC->ODR ^= (1 << 13);
}
