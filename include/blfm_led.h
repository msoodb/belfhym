/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_LED_H
#define BLFM_LED_H

#include <stdint.h>

void blfm_led_init(void);
void blfm_led_set_blink_rate(uint16_t delay_ms);
void blfm_led_blink_task(void *params);

#endif
