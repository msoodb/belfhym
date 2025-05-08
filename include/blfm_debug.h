/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_DEBUG_H
#define BLFM_DEBUG_H

void blfm_debug_init(void);            // Initialize DEBUG sensors
void blfm_debug_status(void);
void blfm_debug_print(void);
void blfm_debug_led_toggle(void);
void blfm_debug_log_flight_data(void);
  
#endif // BLFM_DEBUG_H
