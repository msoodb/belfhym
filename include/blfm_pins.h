
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_PINS_H
#define BLFM_PINS_H

#include "stm32f1xx.h"

// =============================
// === PORT A ==================
// =============================
#define BLFM_MOTOR_LEFT_EN_PORT GPIOA
#define BLFM_MOTOR_LEFT_EN_PIN 0

#define BLFM_MOTOR_RIGHT_EN_PORT GPIOA
#define BLFM_MOTOR_RIGHT_EN_PIN 1

#define BLFM_MODE_BUTTON_PORT GPIOA
#define BLFM_MODE_BUTTON_PIN 4



#define BLFM_SERVO_PWM_PORT GPIOA
#define BLFM_SERVO_PWM_PIN 7  // TIM3_CH2

#define BLFM_IR_REMOTE_PORT GPIOA
#define BLFM_IR_REMOTE_PIN 8

// =============================
// === PORT B ==================
// =============================
#define BLFM_MOTOR_LEFT_IN1_PORT GPIOB
#define BLFM_MOTOR_LEFT_IN1_PIN 0

#define BLFM_MOTOR_LEFT_IN2_PORT GPIOB
#define BLFM_MOTOR_LEFT_IN2_PIN 1

#define BLFM_ULTRASONIC_ECHO_PORT GPIOB
#define BLFM_ULTRASONIC_ECHO_PIN 3

#define BLFM_ULTRASONIC_TRIG_PORT GPIOB
#define BLFM_ULTRASONIC_TRIG_PIN 4

#define BLFM_LED_EXTERNAL_PORT GPIOB
#define BLFM_LED_EXTERNAL_PIN 5

#define BLFM_I2C1_SCL_PORT GPIOB
#define BLFM_I2C1_SCL_PIN 6

#define BLFM_I2C1_SDA_PORT GPIOB
#define BLFM_I2C1_SDA_PIN 7

#define BLFM_MOTOR_RIGHT_IN1_PORT GPIOB
#define BLFM_MOTOR_RIGHT_IN1_PIN 10

#define BLFM_MOTOR_RIGHT_IN2_PORT GPIOB
#define BLFM_MOTOR_RIGHT_IN2_PIN 11


// =============================
// === PORT C ==================
// =============================
#define BLFM_LED_ONBOARD_PORT GPIOC
#define BLFM_LED_ONBOARD_PIN 13


#define BLFM_LED_DEBUG_PORT GPIOC
#define BLFM_LED_DEBUG_PIN 15

#endif /* BLFM_PINS_H */
