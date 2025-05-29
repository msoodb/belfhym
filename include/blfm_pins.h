
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

// Onboard LED: PC13
#define LED_ONBOARD_PORT GPIOC
#define LED_ONBOARD_PIN 13

// External LED: PB5
#define LED_EXTERNAL_PORT GPIOB
#define LED_EXTERNAL_PIN 5

// Ultrasonic 
#define ULTRASONIC_TRIG_PORT GPIOB
#define ULTRASONIC_TRIG_PIN 6
#define ULTRASONIC_ECHO_PORT GPIOB
#define ULTRASONIC_ECHO_PIN 3

// LCD
#define LCD_GPIO GPIOA
#define LCD_RS_PIN 0 // PA0
#define LCD_E_PIN 1  // PA1
#define LCD_D4_PIN 2 // PA2
#define LCD_D5_PIN 3 // PA3
#define LCD_D6_PIN 4 // PA4
#define LCD_D7_PIN 5 // PA5

// ADC
#define ST0248_ADC_CHANNEL 6

#endif /* BLFM_PINS_H */
