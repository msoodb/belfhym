#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

extern char _end;
static char *heap_end;

void *_sbrk(ptrdiff_t incr) {
    if (!heap_end)
        heap_end = &_end;
    char *prev_heap_end = heap_end;
    heap_end += incr;
    return (void *)prev_heap_end;
}

int _write(int file, char *ptr, int len) {
    (void)file; (void)ptr;
    return len;
}

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void)file; (void)ptr; (void)len;
    return 0;
}
