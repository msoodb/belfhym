/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef LIBC_STUB_H
#define LIBC_STUB_H

#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
size_t strlen(const char *s);
void __libc_init_array(void);

#endif
