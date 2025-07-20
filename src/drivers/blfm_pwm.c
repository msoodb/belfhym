
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

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
#include "blfm_pins.h"
#include "blfm_gpio.h"

// =========================
// Configuration: TIM3_CH2
// =========================
#define PWM_TIMER TIM3
#define PWM_CHANNEL_CCR PWM_TIMER->CCR2
#define PWM_RCC_APB1ENR_MASK RCC_APB1ENR_TIM3EN
#define PWM_GPIO_RCC_ENR_MASK RCC_APB2ENR_IOPAEN  // For PA7
#define PWM_TIMER_CLOCK_HZ 72000000
#define PWM_FREQUENCY_HZ 50
#define PWM_PRESCALER (PWM_TIMER_CLOCK_HZ / 1000000 - 1)  // 71 for 1us tick
#define PWM_PERIOD_TICKS (1000000 / PWM_FREQUENCY_HZ)     // 20000 us (20ms)

void blfm_pwm_init(void) {
  // Enable clocks
  RCC->APB2ENR |= PWM_GPIO_RCC_ENR_MASK;  // GPIOA clock for PA7
  RCC->APB1ENR |= PWM_RCC_APB1ENR_MASK;   // TIM3 clock

  // Configure PA7 as AF Push-Pull (TIM3_CH2)
  blfm_gpio_config_alternate_pushpull((uint32_t)BLFM_PWM_PORT, BLFM_PWM_PIN);

  // Timer base setup
  PWM_TIMER->PSC = PWM_PRESCALER;         // 1 MHz tick
  PWM_TIMER->ARR = PWM_PERIOD_TICKS;     // 20 ms period
  PWM_TIMER->CNT = 0;
  PWM_CHANNEL_CCR = 1500;                // Mid-point (90 deg)

  // PWM Mode 1 on Channel 2 (CH2 -> CCMR1, bits 8–15)
  PWM_TIMER->CCMR1 &= ~(TIM_CCMR1_OC2M | TIM_CCMR1_CC2S);
  PWM_TIMER->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);  // PWM Mode 1
  PWM_TIMER->CCMR1 |= TIM_CCMR1_OC2PE;            // Preload enable

  // Enable CH2 output
  PWM_TIMER->CCER &= ~(TIM_CCER_CC2P);
  PWM_TIMER->CCER |= TIM_CCER_CC2E;

  // Auto-reload preload enable
  PWM_TIMER->CR1 |= TIM_CR1_ARPE;

  // Force update event to load registers
  PWM_TIMER->EGR = TIM_EGR_UG;

  // Enable timer
  PWM_TIMER->CR1 |= TIM_CR1_CEN;
}

void blfm_pwm_set_pulse_us(uint16_t us) {
  if (us < 1000)
    us = 1000;
  else if (us > 2000)
    us = 2000;

  PWM_CHANNEL_CCR = us;
}
