/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_UART_H
#define BLFM_UART_H

#include <stdint.h>

void blfm_uart_init(void);
void blfm_uart_write(const char *msg, uint16_t len);

#endif
