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

#include "blfm_types.h"
#include <stdint.h>

void test_single_pin_on_port(void);
void test_all_pins_on_port(void);
void blfm_display_init(void);
void blfm_display_startup_sequence(void);
void blfm_display_apply(const blfm_display_command_t *cmd);

#endif // BLFM_DISPLAY_H
