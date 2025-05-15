#ifndef BLFM_GPIO_H
#define BLFM_GPIO_H

#include "stm32f1xx.h"

typedef enum {
    GPIO_INPUT_FLOATING  = 0x04,
    GPIO_INPUT_PULLUP    = 0x08,
    GPIO_OUTPUT_PP_2MHZ  = 0x02,
    GPIO_OUTPUT_PP_10MHZ = 0x01,
    GPIO_OUTPUT_PP_50MHZ = 0x03,
} GPIOMode_TypeDef;

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIOPinState;

void blfm_gpio_init_pin(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode);
void blfm_gpio_write_pin(GPIO_TypeDef *port, uint16_t pin, GPIOPinState state);
void blfm_gpio_init_alarm_pin(void);
void blfm_gpio_set_alarm_pin_high(void);
void blfm_gpio_set_alarm_pin_low(void);
GPIOPinState blfm_gpio_read_pin(GPIO_TypeDef *port, uint16_t pin);

#endif // BLFM_GPIO_H
