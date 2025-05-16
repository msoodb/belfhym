
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_SAFETY_H
#define BLFM_SAFETY_H

#include <stdint.h>

void blfm_safety_init(void);
void blfm_safety_check(void);
int blfm_safety_get_status(void);

#endif // BLFM_SAFETY_H

