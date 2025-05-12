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

#define MOTOR_GPIO GPIOA
#define MOTOR_PIN 0

void blfm_motor_init(void) {
    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Set PA0 as General purpose output push-pull, 2 MHz
    MOTOR_GPIO->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    MOTOR_GPIO->CRL |= GPIO_CRL_MODE0_1;
}

void blfm_motor_start(void) {
    MOTOR_GPIO->ODR |= (1 << MOTOR_PIN); // Set PA0 high
}

void blfm_motor_stop(void) {
    MOTOR_GPIO->ODR &= ~(1 << MOTOR_PIN); // Set PA0 low
}
