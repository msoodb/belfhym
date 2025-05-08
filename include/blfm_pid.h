/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_PID_H
#define BLFM_PID_H

void blfm_pid_init(void);            // Initialize PID sensors
void blfm_pid_update(void);
void blfm_pid_set_constants(void);

#endif // BLFM_PID_H
