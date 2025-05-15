
#include "blfm_ultrasonic.h"
#include "blfm_board.h"
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "blfm_delay.h"

#define US_PORT     BLFM_ULTRASONIC_PORT
#define TRIG_PIN    BLFM_ULTRASONIC_TRIG_PIN
#define ECHO_PIN    BLFM_ULTRASONIC_ECHO_PIN


void blfm_ultrasonic_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~((0xF << 4) | (0xF << 8));
    GPIOA->CRH |= (0x1 << 4) | (0x8 << 8); // PA9 output, PA10 input
}

// Returns distance in mm (millimeters)
uint16_t blfm_ultrasonic_get_distance_mm(void) {
    uint32_t duration_us = 0;

    // Send 10us pulse
    US_PORT->BSRR = TRIG_PIN;
    delay_us(10);
    US_PORT->BRR = TRIG_PIN;

    // Wait for echo high
    while (!(US_PORT->IDR & ECHO_PIN));
    uint32_t start = blfm_get_us();

    // Wait for echo low
    while (US_PORT->IDR & ECHO_PIN);
    uint32_t end = blfm_get_us();

    duration_us = end - start;

    // Distance in mm: duration_us * 343 / 2 / 1000
    // Simplify: (duration_us * 343) / 2000
    uint16_t distance_mm = (duration_us * 343) / 2000;

    return distance_mm;
}
