#ifndef BLFM_LED_H
#define BLFM_LED_H

#include "stm32f1xx.h"

void blfm_led_init(GPIO_TypeDef *port, uint16_t pin);
void blfm_led_on(GPIO_TypeDef *port, uint16_t pin);
void blfm_led_off(GPIO_TypeDef *port, uint16_t pin);
void blfm_led_toggle(GPIO_TypeDef *port, uint16_t pin);

#endif // BLFM_LED_H
