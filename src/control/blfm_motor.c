/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_motor.h"
#include "stm32f1xx.h"

void blfm_motor_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOB->CRL |= GPIO_CRL_MODE0_0;
}

void blfm_motor_run(void) {
    GPIOB->ODR |= GPIO_ODR_ODR0;
}

void blfm_motor_stop(void) { GPIOB->ODR &= ~GPIO_ODR_ODR0; }
