
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of libc implementation for stm32.
 */

#include "libc_stubs.h"

/* Minimal memset implementation - fills memory with a constant byte */
void *memset(void *dest, int val, size_t len) {
  unsigned char *ptr = dest;
  while (len-- > 0) {
    *ptr++ = (unsigned char)val;
  }
  return dest;
}

/* Minimal memcpy implementation - copies memory (non-overlapping) */
void *memcpy(void *dest, const void *src, size_t len) {
  unsigned char *d = dest;
  const unsigned char *s = src;
  while (len-- > 0) {
    *d++ = *s++;
  }
  return dest;
}

/* Minimal memmove implementation - copies memory (handles overlapping) */
void *memmove(void *dest, const void *src, size_t len) {
  unsigned char *d = dest;
  const unsigned char *s = src;
  
  if (d < s) {
    /* Copy forward */
    while (len-- > 0) {
      *d++ = *s++;
    }
  } else if (d > s) {
    /* Copy backward to handle overlap */
    d += len;
    s += len;
    while (len-- > 0) {
      *--d = *--s;
    }
  }
  return dest;
}

/* Compare two memory blocks */
int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1;
  const unsigned char *p2 = s2;
  
  while (n-- > 0) {
    if (*p1 != *p2) {
      return (*p1 < *p2) ? -1 : 1;
    }
    p1++;
    p2++;
  }
  return 0;
}

/* Get string length */
size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len])
    ++len;
  return len;
}

/* Copy string (unsafe - assumes dest is large enough) */
char *strcpy(char *dest, const char *src) {
  char *d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

/* Safe string copy with size limit */
char *strncpy(char *dest, const char *src, size_t n) {
  char *d = dest;
  
  /* Copy up to n characters from src */
  while (n > 0 && *src) {
    *d++ = *src++;
    n--;
  }
  
  /* Pad with zeros if src was shorter than n */
  while (n > 0) {
    *d++ = '\0';
    n--;
  }
  
  return dest;
}

/* Concatenate strings (unsafe - assumes dest is large enough) */
char *strcat(char *dest, const char *src) {
  char *d = dest + strlen(dest);
  while ((*d++ = *src++))
    ;
  return dest;
}

/* Safe string concatenation with size limit */
char *strncat(char *dest, const char *src, size_t n) {
  char *d = dest + strlen(dest);
  
  while (n > 0 && *src) {
    *d++ = *src++;
    n--;
  }
  *d = '\0';
  
  return dest;
}

/* Compare two strings */
int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/* Compare two strings up to n characters */
int strncmp(const char *s1, const char *s2, size_t n) {
  while (n > 0 && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }
  
  if (n == 0) {
    return 0;
  }
  
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/* Find character in string */
char *strchr(const char *s, int c) {
  while (*s) {
    if (*s == c) {
      return (char *)s;
    }
    s++;
  }
  return (c == '\0') ? (char *)s : NULL;
}

/* Find last occurrence of character in string */
char *strrchr(const char *s, int c) {
  const char *last = NULL;
  
  while (*s) {
    if (*s == c) {
      last = s;
    }
    s++;
  }
  
  if (c == '\0') {
    return (char *)s;
  }
  
  return (char *)last;
}

/* Absolute value of integer */
int abs(int v) {
  return v < 0 ? -v : v;
}

/* Safe string copy with guaranteed null termination */
void safe_strncpy(char *dest, const char *src, size_t max_len) {
  if (max_len == 0) return;
  
  size_t src_len = strlen(src);
  size_t copy_len = (src_len < max_len) ? src_len : max_len - 1;
  memcpy(dest, src, copy_len);
  dest[copy_len] = '\0';  /* Ensure null termination */
}

/* Minimal __libc_init_array - required by linker but does nothing */
void __libc_init_array(void) {
  /* No global/static C++ constructors used, safe to leave empty */
}

