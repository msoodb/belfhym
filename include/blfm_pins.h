
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

#define BLFM_TEMP_SENSOR_PORT GPIOA
#define BLFM_TEMP_SENSOR_PIN 5

#define BLFM_POTENTIOMETER_PORT GPIOA
#define BLFM_POTENTIOMETER_PIN 6
#define BLFM_POTENTIOMETER_ADC_CHANNEL 6

#define BLFM_SERVO_PWM_PORT GPIOA
#define BLFM_SERVO_PWM_PIN 7  // TIM3_CH2

#define BLFM_IR_REMOTE_PORT GPIOA
#define BLFM_IR_REMOTE_PIN 8

#define BLFM_ESP32_UART_TX_PORT GPIOA
#define BLFM_ESP32_UART_TX_PIN 9

#define BLFM_ESP32_UART_RX_PORT GPIOA
#define BLFM_ESP32_UART_RX_PIN 10


// =============================
// === PORT B ==================
// =============================
#define BLFM_MOTOR_LEFT_IN1_PORT GPIOB
#define BLFM_MOTOR_LEFT_IN1_PIN 0

#define BLFM_MOTOR_LEFT_IN2_PORT GPIOB
#define BLFM_MOTOR_LEFT_IN2_PIN 1

#define BLFM_LCD_D6_PORT GPIOB
#define BLFM_LCD_D6_PIN 2    // Moved from PB7 to PB2

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

#define BLFM_LCD_D7_PORT GPIOB
#define BLFM_LCD_D7_PIN 8

#define BLFM_MOTOR_RIGHT_IN1_PORT GPIOB
#define BLFM_MOTOR_RIGHT_IN1_PIN 10

#define BLFM_MOTOR_RIGHT_IN2_PORT GPIOB
#define BLFM_MOTOR_RIGHT_IN2_PIN 11

#define BLFM_LCD_RS_PORT GPIOB
#define BLFM_LCD_RS_PIN 12

#define BLFM_LCD_E_PORT GPIOB
#define BLFM_LCD_E_PIN 13

#define BLFM_LCD_D4_PORT GPIOB
#define BLFM_LCD_D4_PIN 14

#define BLFM_LCD_D5_PORT GPIOB
#define BLFM_LCD_D5_PIN 15


// =============================
// === PORT C ==================
// =============================
#define BLFM_LED_ONBOARD_PORT GPIOC
#define BLFM_LED_ONBOARD_PIN 13

#define BLFM_BUZZER_PORT GPIOC
#define BLFM_BUZZER_PIN 14

#define BLFM_LED_DEBUG_PORT GPIOC
#define BLFM_LED_DEBUG_PIN 15

#endif /* BLFM_PINS_H */
