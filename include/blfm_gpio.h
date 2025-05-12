#ifndef BLFM_GPIO_H
#define BLFM_GPIO_H

#include "stm32f1xx.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} blfm_gpio_pin_t;

void blfm_gpio_init_output(blfm_gpio_pin_t gpio);
void blfm_gpio_init_input(blfm_gpio_pin_t gpio);

void blfm_gpio_set(blfm_gpio_pin_t gpio);
void blfm_gpio_clear(blfm_gpio_pin_t gpio);
void blfm_gpio_toggle(blfm_gpio_pin_t gpio);
int  blfm_gpio_read(blfm_gpio_pin_t gpio);

#endif
