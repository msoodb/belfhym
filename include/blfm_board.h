/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_BOARD_H
#define BLFM_BOARD_H

#define BLFM_THERMAL_ADC_CHANNEL 1
#define BLFM_ULTRASONIC_PORT      GPIOA
#define BLFM_ULTRASONIC_TRIG_PIN  (1U << 9)
#define BLFM_ULTRASONIC_ECHO_PIN  (1U << 10)

void blfm_board_init(void);

#endif // BLFM_BOARD_H
