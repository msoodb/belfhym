
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
#include <stdbool.h>
#include "blfm_types.h"

void blfm_alarm_init(void);
void blfm_alarm_apply(const blfm_alarm_command_t *cmd);

#endif // BLFM_ALARM_H

