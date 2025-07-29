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

// Simple PWM using TIM4 - based on working test
#define PWM_TIMER TIM4
#define PWM_RCC_APB1ENR_MASK RCC_APB1ENR_TIM4EN

// PWM state - simple and direct like the working test
static volatile uint16_t pulse_widths[BLFM_PWM_MAX_CHANNELS] = {1500, 1500, 1500, 1500};
static volatile uint32_t pwm_cycle_counter = 0;

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

// No interrupt handler needed - we'll use direct PWM generation like the working test

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

  // Configure TIM4 for 1us resolution - same as working test
  PWM_TIMER->CR1 = 0;  // Stop timer
  PWM_TIMER->PSC = 71;  // 72MHz / 72 = 1MHz = 1us tick
  PWM_TIMER->ARR = 0xFFFF;  // Max count for free running
  PWM_TIMER->CNT = 0;
  PWM_TIMER->CR1 |= TIM_CR1_CEN;  // Start timer
}

void blfm_pwm_set_pulse_us(uint8_t channel, uint16_t us) {
  if (channel >= BLFM_PWM_MAX_CHANNELS || !channel_enabled[channel]) return;
  
  // Clamp to safe servo range
  if (us < 500) us = 500;
  else if (us > 2500) us = 2500;

  pulse_widths[channel] = us;
}

// Generate one PWM cycle for all enabled channels - FIXED for multiple servos
void blfm_pwm_generate_cycle(void) {
  // Set all enabled channels HIGH simultaneously
  for (uint8_t i = 0; i < BLFM_PWM_MAX_CHANNELS; i++) {
    if (channel_enabled[i]) {
      blfm_gpio_set_pin((uint32_t)channel_pins[i].port, channel_pins[i].pin);
    }
  }
  
  // Record start time for all channels
  uint16_t cycle_start = PWM_TIMER->CNT;
  uint16_t current_time;
  
  // Keep checking and turn off channels at their specific times
  bool channels_active[BLFM_PWM_MAX_CHANNELS];
  for (uint8_t i = 0; i < BLFM_PWM_MAX_CHANNELS; i++) {
    channels_active[i] = channel_enabled[i];
  }
  
  // Monitor all channels until all are turned off
  while (1) {
    current_time = (uint16_t)(PWM_TIMER->CNT - cycle_start);
    bool any_active = false;
    
    // Check each channel if it's time to turn LOW
    for (uint8_t i = 0; i < BLFM_PWM_MAX_CHANNELS; i++) {
      if (channels_active[i] && current_time >= pulse_widths[i]) {
        blfm_gpio_clear_pin((uint32_t)channel_pins[i].port, channel_pins[i].pin);
        channels_active[i] = false;
      }
      if (channels_active[i]) any_active = true;
    }
    
    // Exit when all channels are turned off or max pulse time reached
    if (!any_active || current_time >= 2500) break;
  }
}