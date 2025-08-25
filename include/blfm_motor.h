/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_MOTOR_H
#define BLFM_MOTOR_H

#include "blfm_types.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize motor control system
 * - Configures TIM4 for PWM at 1kHz
 * - Sets up GPIO pins for direction control
 */
void blfm_motor_init(void);

/**
 * Stop both motors immediately
 */
void blfm_motor_stop(void);

/**
 * Apply motor commands
 * @param cmd Motor command structure with speed and direction
 */
void blfm_motor_apply(const blfm_motor_command_t *cmd);

#endif /* BLFM_MOTOR_H */