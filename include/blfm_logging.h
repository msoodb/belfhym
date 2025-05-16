
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_LOGGING_H
#define BLFM_LOGGING_H

#include <stdarg.h>
#include <stdint.h>

void blfm_logging_init(void);
void blfm_logging_log(const char *format, ...);

#endif // BLFM_LOGGING_H

