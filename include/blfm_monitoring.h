
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_MONITORING_H
#define BLFM_MONITORING_H

#include <stdint.h>

void blfm_monitoring_init(void);
void blfm_monitoring_task(void *params);
int blfm_monitoring_check_health(void);

#endif // BLFM_MONITORING_H

