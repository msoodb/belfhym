/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Homa.
 *
 * Homa is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_DEBUG_H
#define BLFM_DEBUG_H

#include <stdint.h>
#include <stdbool.h>

// Public API - Simple and effective debug functions
void blfm_debug_signal_init(void);       // 1 long blink = INIT
void blfm_debug_signal_success(void);    // 2 quick blinks = SUCCESS
void blfm_debug_signal_error(void);      // 5 quick blinks = ERROR
void blfm_debug_flash_quick(uint8_t n);  // N quick flashes
void blfm_debug_blink_byte(uint8_t value);    // Show byte value as blink pattern

#endif // BLFM_DEBUG_H
