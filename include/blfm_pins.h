
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

/*
============================================================
         Belfhym Wiring Map - STM32F103C6T8 (All Pins)
============================================================

PORT A (PA0–PA15)
------------------------------------------------------------
| Pin | Signal                | Description / Usage        |
|-----|-----------------------|----------------------------|
| PA0  | MOTOR_LEFT_EN        | TIM2_CH1 (PWM)             |
| PA1  | MOTOR_RIGHT_EN       | TIM2_CH2 (PWM)             |
| PA2  | // Not used          |                            |
| PA3  | // Not used          |                            |
| PA4  | // Not used          |                            |
| PA5  | TEMP_SENSOR          | ADC5                       |
| PA6  | JOYSTICK_X           | ADC6                       |
| PA7  | JOYSTICK_Y           | ADC7                       |
| PA8  | IR_REMOTE_RECEIVER   | EXTI8                      |
| PA9  | LCD_D6               | LCD data                   |
| PA10 | LCD_D7               | LCD data                   |
| PA11 | // Not used          |                            |
| PA12 | // Not used          |                            |
| PA13 | // Not used (SWDIO)  | Debug                      |
| PA14 | // Not used (SWCLK)  | Debug                      |
| PA15 | // Not used          |                            |

PORT B (PB0–PB15)
------------------------------------------------------------
| PB0  | MOTOR_LEFT_IN1       | GPIO output                |
| PB1  | MOTOR_LEFT_IN2       | GPIO output                |
| PB3  | ULTRASONIC_ECHO      | Input echo                 |
| PB4  | SERVO_PWM            | TIM3_CH1                   |
| PB5  | LED_EXTERNAL         | GPIO output                |
| PB6  | ULTRASONIC_TRIG      | Output trigger             |
| PB7  | LED_DEBUG            | GPIO output                |
| PB10 | MOTOR_RIGHT_IN2      | GPIO output                |
| PB11 | MOTOR_RIGHT_IN1      | GPIO output                |
| PB12 | LCD_RS               | LCD control                |
| PB13 | LCD_E                | LCD control                |
| PB14 | LCD_D4               | LCD data                   |
| PB15 | LCD_D5               | LCD data                   |

PORT C (PC0–PC15)
------------------------------------------------------------
| PC0  | MODE_BUTTON          | Digital input (EXTI0)      |
| PC1  | JOYSTICK_BUTTON      | Digital input (EXTI)       |
| PC13 | LED_ONBOARD          | Built-in LED               |
| PC14 | BUZZER               | GPIO or PWM optional       |
============================================================
*/

#include "stm32f1xx.h"

// --- PORT A ---
#define BLFM_MOTOR_LEFT_EN_PORT       GPIOA
#define BLFM_MOTOR_LEFT_EN_PIN        0

#define BLFM_MOTOR_RIGHT_EN_PORT      GPIOA
#define BLFM_MOTOR_RIGHT_EN_PIN       1

#define BLFM_TEMP_SENSOR_PORT         GPIOA
#define BLFM_TEMP_SENSOR_PIN          5

#define BLFM_JOYSTICK_X_PORT          GPIOA
#define BLFM_JOYSTICK_X_PIN           6

#define BLFM_JOYSTICK_Y_PORT          GPIOA
#define BLFM_JOYSTICK_Y_PIN           7

#define BLFM_IR_REMOTE_PORT           GPIOA
#define BLFM_IR_REMOTE_PIN            8

#define BLFM_LCD_D6_PORT              GPIOA
#define BLFM_LCD_D6_PIN               9

#define BLFM_LCD_D7_PORT              GPIOA
#define BLFM_LCD_D7_PIN               10

// --- PORT B ---
#define BLFM_MOTOR_LEFT_IN1_PORT      GPIOB
#define BLFM_MOTOR_LEFT_IN1_PIN       0

#define BLFM_MOTOR_LEFT_IN2_PORT      GPIOB
#define BLFM_MOTOR_LEFT_IN2_PIN       1

#define BLFM_MOTOR_RIGHT_IN1_PORT     GPIOB
#define BLFM_MOTOR_RIGHT_IN1_PIN      11

#define BLFM_MOTOR_RIGHT_IN2_PORT     GPIOB
#define BLFM_MOTOR_RIGHT_IN2_PIN      10

#define BLFM_ULTRASONIC_ECHO_PORT     GPIOB
#define BLFM_ULTRASONIC_ECHO_PIN      3

#define BLFM_SERVO_PWM_PORT           GPIOB
#define BLFM_SERVO_PWM_PIN            4

#define BLFM_LED_EXTERNAL_PORT        GPIOB
#define BLFM_LED_EXTERNAL_PIN         5

#define BLFM_ULTRASONIC_TRIG_PORT     GPIOB
#define BLFM_ULTRASONIC_TRIG_PIN      6

#define BLFM_LED_DEBUG_PORT           GPIOB
#define BLFM_LED_DEBUG_PIN            7

#define BLFM_LCD_RS_PORT              GPIOB
#define BLFM_LCD_RS_PIN               12

#define BLFM_LCD_E_PORT               GPIOB
#define BLFM_LCD_E_PIN                13

#define BLFM_LCD_D4_PORT              GPIOB
#define BLFM_LCD_D4_PIN               14

#define BLFM_LCD_D5_PORT              GPIOB
#define BLFM_LCD_D5_PIN               15

// --- PORT C ---
#define BLFM_MODE_BUTTON_PORT         GPIOC
#define BLFM_MODE_BUTTON_PIN          0

#define BLFM_JOYSTICK_BUTTON_PORT     GPIOC
#define BLFM_JOYSTICK_BUTTON_PIN      1

#define BLFM_LED_ONBOARD_PORT         GPIOC
#define BLFM_LED_ONBOARD_PIN          13

#define BLFM_BUZZER_PORT              GPIOC
#define BLFM_BUZZER_PIN               14

#endif /* BLFM_PINS_H */
