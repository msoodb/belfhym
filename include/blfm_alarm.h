
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_ALARM_H
#define BLFM_ALARM_H

#include <stdint.h>

void blfm_alarm_init(void);
void blfm_alarm_trigger(void);
void blfm_alarm_clear(void);

#endif // BLFM_ALARM_H

