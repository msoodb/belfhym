#ifndef BLFM_UART_H
#define BLFM_UART_H

#include <stdint.h>

void blfm_uart_init(void);
void blfm_uart_write(const char *msg, uint16_t len);

#endif
