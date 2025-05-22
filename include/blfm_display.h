/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_DISPLAY_H
#define BLFM_DISPLAY_H

#include <stdint.h>

void blfm_display_init(void);
void blfm_display_startup_sequence(void);
void blfm_lcd_send_command(uint8_t cmd);
void blfm_lcd_send_data(uint8_t data);

#endif // BLFM_DISPLAY_H
