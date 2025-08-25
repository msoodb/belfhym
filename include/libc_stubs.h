
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of libc implementation for stm32.
 */

#ifndef LIBC_STUBS_H
#define LIBC_STUBS_H

#include <stddef.h>

/* Memory functions */
void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len);
int memcmp(const void *s1, const void *s2, size_t n);

/* String functions */
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

/* Math functions */
int abs(int v);

/* Safe string functions */
void safe_strncpy(char *dest, const char *src, size_t max_len);

/* Special init function required by linker */
void __libc_init_array(void);

#endif /* LIBC_STUBS_H */

