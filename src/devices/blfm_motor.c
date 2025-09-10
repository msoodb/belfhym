/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_motor.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"
#include <stdbool.h>

#define PWM_FREQUENCY 1000
#define PWM_PERIOD 1000
#define MOTOR_MAX_SPEED 255

void blfm_motor_init(void) {
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    
    blfm_gpio_config_peripheral((uint32_t)GPIOB, BLFM_MOTOR_ENA_PIN);
    blfm_gpio_config_peripheral((uint32_t)GPIOB, BLFM_MOTOR_ENB_PIN);
    
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    
    TIM4->PSC = 71;
    TIM4->ARR = PWM_PERIOD - 1;
    TIM4->CCR1 = 0;
    TIM4->CCR2 = 0;
    
    TIM4->CCMR1 = (6 << 4) | (6 << 12);
    TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM4->CR1 = TIM_CR1_CEN;
}

void blfm_motor_stop(void) {
    TIM4->CCR1 = 0;
    TIM4->CCR2 = 0;
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
}

void blfm_motor_apply(const blfm_motor_command_t *cmd) {
    if (!cmd) return;
    
    uint16_t left_speed = (cmd->left.speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : cmd->left.speed;
    uint16_t right_speed = (cmd->right.speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : cmd->right.speed;
    
    uint16_t left_pwm = (left_speed * PWM_PERIOD) / MOTOR_MAX_SPEED;
    uint16_t right_pwm = (right_speed * PWM_PERIOD) / MOTOR_MAX_SPEED;
    
    if (left_speed == 0) {
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    } else if (cmd->left.direction == 0) {
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    } else {
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    }
    
    if (right_speed == 0) {
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    } else if (cmd->right.direction == 0) {
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    } else {
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    }
    
    TIM4->CCR1 = left_pwm;
    TIM4->CCR2 = right_pwm;
}

