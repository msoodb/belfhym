
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_motor.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"
#include "blfm_config.h"
#include "blfm_pins.h"

// ==== Motor Pin Configuration ====

// Left motor
#define LEFT_DIR_PORT GPIOA_BASE
#define LEFT_DIR_PIN1 0
#define LEFT_DIR_PIN2 1
#define LEFT_PWM_CCR TIM3->CCR1

// Right motor
#define RIGHT_DIR_PORT GPIOA_BASE
#define RIGHT_DIR_PIN1 2
#define RIGHT_DIR_PIN2 3
#define RIGHT_PWM_CCR TIM3->CCR2

static void blfm_motor_set_side(const blfm_single_motor_command_t *cmd, bool is_left);

void blfm_motor_init(void) {
  // === Configure motor direction pins as output ===
  blfm_gpio_config_output(LEFT_DIR_PORT, LEFT_DIR_PIN1);
  blfm_gpio_config_output(LEFT_DIR_PORT, LEFT_DIR_PIN2);
  blfm_gpio_config_output(RIGHT_DIR_PORT, RIGHT_DIR_PIN1);
  blfm_gpio_config_output(RIGHT_DIR_PORT, RIGHT_DIR_PIN2);

  // === Enable TIM3 and configure PWM on CH1 (PA6) and CH2 (PA7) ===
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // Configure PA6 and PA7 as alternate function push-pull
  GPIOA->CRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
  GPIOA->CRL |=  ((0xB << (6 * 4)) | (0xB << (7 * 4)));  // AF PP, 50MHz

  TIM3->PSC = 72 - 1;   // 72 MHz / 72 = 1 MHz
  TIM3->ARR = 255;      // 8-bit resolution

  TIM3->CCR1 = 0;       // Initial speed = 0
  TIM3->CCR2 = 0;

  // CH1 and CH2 to PWM Mode 1 with preload
  TIM3->CCMR1 |= (6 << 4) | (1 << 3);     // CH1
  TIM3->CCMR1 |= (6 << 12) | (1 << 11);   // CH2

  TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // Enable CH1 and CH2 outputs
  TIM3->CR1 |= TIM_CR1_CEN;                    // Start timer
}

void blfm_motor_apply(const blfm_motor_command_t *cmd) {
  if (!cmd)
    return;

  blfm_motor_set_side(&cmd->left, true);
  blfm_motor_set_side(&cmd->right, false);
}

static void blfm_motor_set_side(const blfm_single_motor_command_t *cmd, bool is_left) {
  if (!cmd)
    return;

  uint32_t dir_port;
  uint8_t pin1, pin2;
  volatile uint32_t *pwm_ccr;

  if (is_left) {
    dir_port = LEFT_DIR_PORT;
    pin1 = LEFT_DIR_PIN1;
    pin2 = LEFT_DIR_PIN2;
    pwm_ccr = &LEFT_PWM_CCR;
  } else {
    dir_port = RIGHT_DIR_PORT;
    pin1 = RIGHT_DIR_PIN1;
    pin2 = RIGHT_DIR_PIN2;
    pwm_ccr = &RIGHT_PWM_CCR;
  }

  if (cmd->direction == 0) {
    // Forward
    blfm_gpio_set_pin(dir_port, pin1);
    blfm_gpio_clear_pin(dir_port, pin2);
  } else {
    // Backward
    blfm_gpio_clear_pin(dir_port, pin1);
    blfm_gpio_set_pin(dir_port, pin2);
  }

  // Clamp speed to 0–255
  *pwm_ccr = cmd->speed;

  //blfm_gpio_set_pin((uint32_t)LED_DEBUG_PORT, LED_DEBUG_PIN);
 
}
