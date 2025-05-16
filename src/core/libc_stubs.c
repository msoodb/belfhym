
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include <stddef.h>
#include <stdint.h>

// Minimal memset implementation
void *memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}

// Minimal memcpy implementation
void *memcpy(void *dest, const void *src, size_t len) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (len-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

// Minimal __libc_init_array (does nothing)
void __libc_init_array(void) {
    // No global/static C++ constructors used, safe to leave empty.
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) ++len;
    return len;
}
