
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_BUTTON_H
#define BLFM_BUTTON_H

#include <stdint.h>

void blfm_button_init(uint32_t port, uint32_t pin);
int blfm_button_is_pressed(uint32_t port, uint32_t pin);

#endif // BLFM_BUTTON_H

