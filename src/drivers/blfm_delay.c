#include "stm32f1xx.h"
#include "blfm_delay.h"

// Variable to store microseconds
static volatile uint32_t g_ticks_us = 0;

// Initialize SysTick timer for 1us precision
void blfm_delay_init(void) {
    // Configure SysTick for 1 MHz (1us) precision using the system clock
    SysTick_Config(SystemCoreClock / 1000000);  // 1us resolution
}

// Delay in microseconds
void delay_us(uint32_t us) {
    g_ticks_us = us;
    while (g_ticks_us > 0);
}

// Delay in milliseconds
void delay_ms(uint32_t ms) {
    while (ms > 0) {
        delay_us(1000);
        ms--;
    }
}

// Get the current time in microseconds (since SysTick started)
uint32_t blfm_get_us(void) {
    return (SystemCoreClock / 1000000) * g_ticks_us;
}
