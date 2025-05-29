
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

// ==== Motor Pin Configuration ====

// Left motor
#define LEFT_DIR_PORT GPIOA_BASE
#define LEFT_DIR_PIN1 0
#define LEFT_DIR_PIN2 1
#define LEFT_PWM_CCR TIM2->CCR1

// Right motor
#define RIGHT_DIR_PORT GPIOA_BASE
#define RIGHT_DIR_PIN1 2
#define RIGHT_DIR_PIN2 3
#define RIGHT_PWM_CCR TIM2->CCR2

static void blfm_motor_set_side(const blfm_single_motor_command_t *cmd,
                                bool is_left);

void blfm_motor_init(void) {
  // === Configure motor direction pins as output ===
  blfm_gpio_config_output(LEFT_DIR_PORT, LEFT_DIR_PIN1);
  blfm_gpio_config_output(LEFT_DIR_PORT, LEFT_DIR_PIN2);
  blfm_gpio_config_output(RIGHT_DIR_PORT, RIGHT_DIR_PIN1);
  blfm_gpio_config_output(RIGHT_DIR_PORT, RIGHT_DIR_PIN2);

  // === Enable TIM2 and configure PWM on CH1 & CH2 ===
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  TIM2->PSC = 72 - 1; // 72 MHz / 72 = 1 MHz
  TIM2->ARR = 255;    // 8-bit PWM
  TIM2->CCR1 = 0;     // Left motor speed
  TIM2->CCR2 = 0;     // Right motor speed

  // Set CH1 and CH2 to PWM mode 1
  TIM2->CCMR1 |= (6 << 4) | (1 << 3);   // CH1: PWM mode 1, enable preload
  TIM2->CCMR1 |= (6 << 12) | (1 << 11); // CH2: PWM mode 1, enable preload

  TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // Enable CH1 and CH2 output
  TIM2->CR1 |= TIM_CR1_CEN;                    // Enable TIM2
}

void blfm_motor_apply(const blfm_motor_command_t *cmd) {
  if (!cmd)
    return;

  blfm_motor_set_side(&cmd->left, true);
  blfm_motor_set_side(&cmd->right, false);
}

static void blfm_motor_set_side(const blfm_single_motor_command_t *cmd,
                                bool is_left) {
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

  // Set PWM duty cycle
  *pwm_ccr = cmd->speed; // speed = 0–255
}
