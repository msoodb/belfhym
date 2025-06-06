
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_pwm.h"
#include "stm32f1xx.h"

// --- PWM config for TIM2_CH2 (PA1) ---
// TIM2: 72MHz -> PSC=71 --> 1MHz (1us per tick)
// ARR=20000 -> 50Hz (20ms period)

#define PWM_TIMER TIM4
#define PWM_CHANNEL_CCR PWM_TIMER->CCR3
#define PWM_RCC_APB1ENR_MASK RCC_APB1ENR_TIM4EN
#define PWM_GPIO_RCC_ENR_MASK RCC_APB2ENR_IOPBEN
#define PWM_GPIO_PORT GPIOB
#define PWM_GPIO_PIN 8
#define PWM_GPIO_MODE_BITS (0xB)  // AF Push-Pull, 50 MHz
#define PWM_TIMER_CLOCK_HZ 72000000
#define PWM_FREQUENCY_HZ 50
#define PWM_TICK_US 1
#define PWM_PRESCALER (PWM_TIMER_CLOCK_HZ / 1000000 - 1)  // 71
#define PWM_PERIOD_TICKS (1000000 / PWM_FREQUENCY_HZ)     // 20000

void blfm_pwm_init(void) {
  // 1. Enable clocks
  RCC->APB2ENR |= PWM_GPIO_RCC_ENR_MASK;  // GPIOB
  RCC->APB1ENR |= PWM_RCC_APB1ENR_MASK;   // TIM4

  // 2. Configure PB8 as AF Push-Pull, 50 MHz (CRH bits 0-3 for pin 8)
  PWM_GPIO_PORT->CRH &= ~(0xF << ((PWM_GPIO_PIN - 8) * 4));
  PWM_GPIO_PORT->CRH |= (PWM_GPIO_MODE_BITS << ((PWM_GPIO_PIN - 8) * 4));

  // 3. Timer base setup: PSC, ARR, initial pulse width
  PWM_TIMER->PSC = PWM_PRESCALER;
  PWM_TIMER->ARR = PWM_PERIOD_TICKS;
  PWM_CHANNEL_CCR = 1500;

  // 4. Configure PWM Mode 1 on channel 3
  PWM_TIMER->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_CC3S);
  PWM_TIMER->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);
  PWM_TIMER->CCMR2 |= TIM_CCMR2_OC3PE;  // Enable CCR3 preload

  // 5. Generate an update event to apply registers immediately
  PWM_TIMER->EGR = TIM_EGR_UG;

  // 6. Enable channel 3 output and timer
  PWM_TIMER->CCER |= TIM_CCER_CC3E;
  PWM_TIMER->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
}

void blfm_pwm_set_pulse_us(uint16_t us) {
  if (us < 1000)
    us = 1000;
  else if (us > 2000)
    us = 2000;
  PWM_CHANNEL_CCR = us;
}
