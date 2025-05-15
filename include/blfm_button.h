#ifndef BLFM_BUTTON_H
#define BLFM_BUTTON_H

#include "stm32f1xx.h"
#include <stdbool.h>

void blfm_button_init(GPIO_TypeDef *port, uint16_t pin);
bool blfm_button_is_pressed(GPIO_TypeDef *port, uint16_t pin);

#endif // BLFM_BUTTON_H
