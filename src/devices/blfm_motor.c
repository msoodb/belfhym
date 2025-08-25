/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_motor.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"
#include <stdbool.h>

/* TIM4 PWM Configuration */
#define PWM_FREQUENCY 1000      /* 1kHz PWM frequency */
#define PWM_PERIOD 1000         /* PWM period for 1kHz (1MHz/1000 = 1kHz) */
#define MOTOR_MAX_SPEED 255     /* Maximum speed value */

/* Initialize motor control system */
void blfm_motor_init(void) {
    /* Configure direction control pins as outputs */
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    
    /* Configure PWM pins (ENA=PB6=TIM4_CH1, ENB=PB7=TIM4_CH2) as alternate function */
    blfm_gpio_config_peripheral((uint32_t)GPIOB, BLFM_MOTOR_ENA_PIN);  /* ENA - TIM4_CH1 */
    blfm_gpio_config_peripheral((uint32_t)GPIOB, BLFM_MOTOR_ENB_PIN);  /* ENB - TIM4_CH2 */
    
    /* Stop motors initially */
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    
    /* Enable TIM4 clock */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    
    /* Configure TIM4 for PWM at 1kHz */
    TIM4->PSC = 71;                    /* Prescaler: 72MHz/72 = 1MHz */
    TIM4->ARR = PWM_PERIOD - 1;        /* Period: 1MHz/1000 = 1kHz */
    TIM4->CCR1 = 0;                    /* Left motor: 0% duty */
    TIM4->CCR2 = 0;                    /* Right motor: 0% duty */
    
    /* Configure PWM mode 1 */
    TIM4->CCMR1 = (6 << 4) | (6 << 12);  /* CH1 and CH2 in PWM mode 1 */
    TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;  /* Enable CH1 and CH2 */
    TIM4->CR1 = TIM_CR1_CEN;          /* Enable timer */
}

/* Stop both motors immediately */
void blfm_motor_stop(void) {
    TIM4->CCR1 = 0;  /* ENA = 0% */
    TIM4->CCR2 = 0;  /* ENB = 0% */
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
    blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
}

/* Apply motor commands */
void blfm_motor_apply(const blfm_motor_command_t *cmd) {
    if (!cmd) return;
    
    /* Clamp speeds to max value */
    uint16_t left_speed = (cmd->left.speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : cmd->left.speed;
    uint16_t right_speed = (cmd->right.speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : cmd->right.speed;
    
    /* Convert speed to PWM duty cycle (0-1000) */
    uint16_t left_pwm = (left_speed * PWM_PERIOD) / MOTOR_MAX_SPEED;
    uint16_t right_pwm = (right_speed * PWM_PERIOD) / MOTOR_MAX_SPEED;
    
    /* Set left motor direction (reversed wiring compensation) */
    if (left_speed == 0) {
        /* Stop */
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    } else if (cmd->left.direction == 0) {
        /* Forward - but left motor wired backwards */
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    } else {
        /* Backward - but left motor wired backwards */
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN1_PORT, BLFM_MOTOR_IN1_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN2_PORT, BLFM_MOTOR_IN2_PIN);
    }
    
    /* Set right motor direction (reversed wiring compensation) */
    if (right_speed == 0) {
        /* Stop */
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    } else if (cmd->right.direction == 0) {
        /* Forward - but right motor wired backwards */
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    } else {
        /* Backward - but right motor wired backwards */
        blfm_gpio_set_pin((uint32_t)BLFM_MOTOR_IN3_PORT, BLFM_MOTOR_IN3_PIN);
        blfm_gpio_clear_pin((uint32_t)BLFM_MOTOR_IN4_PORT, BLFM_MOTOR_IN4_PIN);
    }
    
    /* Set PWM duty cycles */
    TIM4->CCR1 = left_pwm;   /* ENA - Left Motor Speed */
    TIM4->CCR2 = right_pwm;  /* ENB - Right Motor Speed */
}

