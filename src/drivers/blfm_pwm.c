/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_pwm.h"
#include "blfm_config.h"
#include "stm32f1xx.h"
#include "blfm_pins.h"
#include "blfm_gpio.h"

// Software PWM using TIM4 as timebase
#define PWM_TIMER TIM4
#define PWM_RCC_APB1ENR_MASK RCC_APB1ENR_TIM4EN
#define PWM_TIMER_CLOCK_HZ 72000000
#define PWM_PERIOD_US 20000  // 20ms period for 50Hz
#define PWM_PRESCALER (PWM_TIMER_CLOCK_HZ / 1000000 - 1)  // 1us tick

// Software PWM state
static volatile uint32_t pwm_counter = 0;
static volatile uint16_t pulse_widths[BLFM_PWM_MAX_CHANNELS] = {1500, 1500, 1500, 1500};

// Channel enabled flags based on individual servo config
static const bool channel_enabled[BLFM_PWM_MAX_CHANNELS] = {
#if BLFM_ENABLED_SERVO1
  true,   // Channel 0 - PA0
#else
  false,
#endif
#if BLFM_ENABLED_SERVO2
  true,   // Channel 1 - PA1
#else
  false,
#endif
#if BLFM_ENABLED_SERVO3
  true,   // Channel 2 - PA2
#else
  false,
#endif
#if BLFM_ENABLED_SERVO4
  true    // Channel 3 - PA3
#else
  false
#endif
};

// GPIO configuration for each channel
typedef struct {
  GPIO_TypeDef *port;
  uint8_t pin;
} gpio_pin_config_t;

static const gpio_pin_config_t channel_pins[BLFM_PWM_MAX_CHANNELS] = {
  {BLFM_SERVO1_PWM_PORT, BLFM_SERVO1_PWM_PIN},  // PA0
  {BLFM_SERVO2_PWM_PORT, BLFM_SERVO2_PWM_PIN},  // PA1
  {BLFM_SERVO3_PWM_PORT, BLFM_SERVO3_PWM_PIN},  // PA2
  {BLFM_SERVO4_PWM_PORT, BLFM_SERVO4_PWM_PIN}   // PA3
};

// Timer interrupt handler for software PWM - optimized for servo control
void TIM4_IRQHandler(void) {
  if (PWM_TIMER->SR & TIM_SR_UIF) {
    PWM_TIMER->SR &= ~TIM_SR_UIF;  // Clear interrupt flag
    
    static uint8_t pwm_state = 0;  // 0=start period, 1-4=channel end times
    static uint16_t sorted_times[BLFM_PWM_MAX_CHANNELS];
    static uint8_t sorted_channels[BLFM_PWM_MAX_CHANNELS];
    static uint8_t next_event = 0;
    
    if (pwm_state == 0) {
      // Start of new PWM period - set enabled pins HIGH and sort end times
      for (int i = 0; i < BLFM_PWM_MAX_CHANNELS; i++) {
        if (channel_enabled[i]) {
          blfm_gpio_set_pin((uint32_t)channel_pins[i].port, channel_pins[i].pin);
        }
        sorted_times[i] = pulse_widths[i];
        sorted_channels[i] = i;
      }
      
      // Simple bubble sort for 4 elements
      for (int i = 0; i < BLFM_PWM_MAX_CHANNELS - 1; i++) {
        for (int j = 0; j < BLFM_PWM_MAX_CHANNELS - 1 - i; j++) {
          if (sorted_times[j] > sorted_times[j + 1]) {
            uint16_t temp_time = sorted_times[j];
            uint8_t temp_ch = sorted_channels[j];
            sorted_times[j] = sorted_times[j + 1];
            sorted_channels[j] = sorted_channels[j + 1];
            sorted_times[j + 1] = temp_time;
            sorted_channels[j + 1] = temp_ch;
          }
        }
      }
      
      next_event = 0;
      pwm_state = 1;
      
      // Set timer for first channel end time
      if (next_event < BLFM_PWM_MAX_CHANNELS) {
        PWM_TIMER->ARR = sorted_times[next_event] - 1;
      }
    } else {
      // Turn off the enabled channel(s) that should end now
      while (next_event < BLFM_PWM_MAX_CHANNELS && sorted_times[next_event] <= (PWM_TIMER->ARR + 1)) {
        uint8_t ch = sorted_channels[next_event];
        if (channel_enabled[ch]) {
          blfm_gpio_clear_pin((uint32_t)channel_pins[ch].port, channel_pins[ch].pin);
        }
        next_event++;
      }
      
      if (next_event < BLFM_PWM_MAX_CHANNELS) {
        // Set timer for next channel end time
        PWM_TIMER->ARR = sorted_times[next_event] - 1;
      } else {
        // All channels done, wait for next period
        PWM_TIMER->ARR = PWM_PERIOD_US - 1;
        pwm_state = 0;
      }
    }
  }
}

void blfm_pwm_init(void) {
  // Enable clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // GPIOA for PA0-PA3
  RCC->APB1ENR |= PWM_RCC_APB1ENR_MASK;   // TIM4 clock

  // Configure GPIO pins as outputs for enabled channels only
  for (int i = 0; i < BLFM_PWM_MAX_CHANNELS; i++) {
    if (channel_enabled[i]) {
      blfm_gpio_config_output((uint32_t)channel_pins[i].port, channel_pins[i].pin);
      blfm_gpio_clear_pin((uint32_t)channel_pins[i].port, channel_pins[i].pin);  // Start LOW
    }
  }

  // Configure TIM4 for optimized software PWM
  PWM_TIMER->CR1 = 0;  // Stop timer
  PWM_TIMER->PSC = PWM_PRESCALER;  // 1us tick
  PWM_TIMER->ARR = PWM_PERIOD_US - 1;  // Start with full period
  PWM_TIMER->CNT = 0;
  PWM_TIMER->DIER |= TIM_DIER_UIE;  // Enable update interrupt

  // Enable TIM4 interrupt in NVIC
  NVIC_EnableIRQ(TIM4_IRQn);
  NVIC_SetPriority(TIM4_IRQn, 6);  // Lower priority to not block other interrupts

  // Start timer
  PWM_TIMER->CR1 |= TIM_CR1_CEN;
}

void blfm_pwm_set_pulse_us(uint8_t channel, uint16_t us) {
  if (channel >= BLFM_PWM_MAX_CHANNELS || !channel_enabled[channel]) return;
  
  // Allow wider range for SG90 servo testing
  if (us < 500) us = 500;
  else if (us > 2500) us = 2500;

  pulse_widths[channel] = us;
}