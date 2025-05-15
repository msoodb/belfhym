#ifndef LIBC_STUB_H
#define LIBC_STUB_H

#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
size_t strlen(const char *s);
void __libc_init_array(void);

#endif
