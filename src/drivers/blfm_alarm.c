#include "blfm_alarm.h"
#include "stm32f1xx.h"

void blfm_alarm_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
    GPIOB->CRH |= GPIO_CRH_MODE12_0;
}

void blfm_alarm_on(void) {
    GPIOB->ODR |= GPIO_ODR_ODR12;
}

void blfm_alarm_off(void) {
    GPIOB->ODR &= ~GPIO_ODR_ODR12;
}
