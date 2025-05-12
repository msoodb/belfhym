#ifndef BLFM_DELAY_H
#define BLFM_DELAY_H

#include <stdint.h>

// Initialize SysTick for microsecond delays
void blfm_delay_init(void);

// Delay in microseconds
void delay_us(uint32_t us);

// Delay in milliseconds
void delay_ms(uint32_t ms);

// Get the current time in microseconds since SysTick started
uint32_t blfm_get_us(void);

#endif
