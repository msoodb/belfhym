
#include "blfm_debug.h"
#include "blfm_uart.h"
#include "libc_stub.h"

void blfm_debug_init(void) {
    blfm_uart_init();
}

void blfm_debug_print(const char *msg) { blfm_uart_write(msg, strlen(msg)); }
