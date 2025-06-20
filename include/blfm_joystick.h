
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_JOYSTICK_H
#define BLFM_JOYSTICK_H

#include "FreeRTOS.h"
#include "blfm_types.h"
#include "queue.h"
#include <stdint.h>
#include "stm32f1xx.h"  // For GPIO_TypeDef *

// Joystick GPIO pins and ports

#define JOYSTICK_TASK_STACK_SIZE 128
//#define JOYSTICK_TASK_PRIORITY 2
#define JOYSTICK_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define JOYSTICK_QUEUE_LENGTH 1

// ADC channels for joystick X and Y
//#define JOYSTICK_X_CHANNEL 8  // PB0 = ADC channel 8
//#define JOYSTICK_Y_CHANNEL 9  // PB1 = ADC channel 9

// Joystick interface
void blfm_joystick_init(void);
bool blfm_joystick_read(blfm_joystick_data_t *out);

#endif // BLFM_JOYSTICK_H
