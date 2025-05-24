
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

#include "blfm_types.h"

void blfm_led_init(void);
void blfm_led_apply(const blfm_led_command_t *cmd);

#endif // BLFM_ALARM_H
