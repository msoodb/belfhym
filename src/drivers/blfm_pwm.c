
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

#define PWM_TIMER TIM4
#define PWM_CHANNEL_CCR PWM_TIMER->CCR3
#define PWM_RCC_APB1ENR_MASK RCC_APB1ENR_TIM4EN
#define PWM_GPIO_RCC_ENR_MASK RCC_APB2ENR_IOPBEN
#define PWM_TIMER_CLOCK_HZ 72000000
#define PWM_FREQUENCY_HZ 50
#define PWM_PRESCALER (PWM_TIMER_CLOCK_HZ / 1000000 - 1)  // 71
#define PWM_PERIOD_TICKS (1000000 / PWM_FREQUENCY_HZ)     // 20000

void blfm_pwm_init(void) {
  // Enable clocks
  RCC->APB2ENR |= PWM_GPIO_RCC_ENR_MASK;  // GPIOB clock
  RCC->APB1ENR |= PWM_RCC_APB1ENR_MASK;   // TIM4 clock

  // Configure PB8 as AF Push-Pull
  blfm_gpio_config_alternate_pushpull((uint32_t)BLFM_SERVO_PWM_PORT, BLFM_SERVO_PWM_PIN);

  // Timer base
  PWM_TIMER->PSC = PWM_PRESCALER;
  PWM_TIMER->ARR = PWM_PERIOD_TICKS;
  PWM_TIMER->CNT = 0;
  PWM_CHANNEL_CCR = 1500;

  // PWM Mode 1
  PWM_TIMER->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_CC3S);
  PWM_TIMER->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);
  PWM_TIMER->CCMR2 |= TIM_CCMR2_OC3PE;

  // Enable output
  PWM_TIMER->CCER &= ~(TIM_CCER_CC3P);
  PWM_TIMER->CCER |= TIM_CCER_CC3E;

  // Auto-reload preload
  PWM_TIMER->CR1 |= TIM_CR1_ARPE;

  // Update event
  PWM_TIMER->EGR = TIM_EGR_UG;

  // Start timer
  PWM_TIMER->CR1 |= TIM_CR1_CEN;
}

void blfm_pwm_set_pulse_us(uint16_t us) {
  if (us < 1000)
    us = 1000;
  else if (us > 2000)
    us = 2000;
  PWM_CHANNEL_CCR = us;
  blfm_gpio_toggle_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
}
