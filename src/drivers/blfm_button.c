
#include "blfm_button.h"
#include "stm32f1xx.h"

void blfm_button_init(void) {
    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA0 as input with pull-up
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // Input with pull-up/down
    GPIOA->ODR |= GPIO_ODR_ODR0;   // Pull-up enabled
}

bool blfm_button_is_pressed(void) {
    return !(GPIOA->IDR & GPIO_IDR_IDR0);  // Active-low button
}
