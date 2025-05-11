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
    // Enable GPIOA and TIM1 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN;

    // Configure PA8 (TIM1_CH1) as alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
    GPIOA->CRH |= (GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0); // Output mode, max speed 50MHz
    GPIOA->CRH |= GPIO_CRH_CNF8_1; // Alternate function push-pull

    // Configure TIM1 for PWM mode
    TIM1->PSC = 72 - 1;      // Prescaler: 72MHz / 72 = 1MHz
    TIM1->ARR = 1000 - 1;    // Auto-reload: 1000 for 1kHz PWM
    TIM1->CCR1 = 0;          // Initial duty cycle 0%
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // Preload enable
    TIM1->CCER |= TIM_CCER_CC1E; // Enable output on CH1
    TIM1->BDTR |= TIM_BDTR_MOE; // Main output enable
    TIM1->CR1 |= TIM_CR1_ARPE; // Auto-reload preload
    TIM1->CR1 |= TIM_CR1_CEN;  // Enable timer
}

void blfm_motor_set_pwm(uint16_t duty) {
    if (duty > 1000) duty = 1000;
    TIM1->CCR1 = duty;
}
