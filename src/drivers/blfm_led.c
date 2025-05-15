#include "blfm_led.h"
#include "blfm_gpio.h"

void blfm_led_init(GPIO_TypeDef *port, uint16_t pin) {
    blfm_gpio_init_pin(port, pin, GPIO_OUTPUT_PP_2MHZ);
}

void blfm_led_on(GPIO_TypeDef *port, uint16_t pin) {
    blfm_gpio_write_pin(port, pin, GPIO_PIN_SET);
}

void blfm_led_off(GPIO_TypeDef *port, uint16_t pin) {
    blfm_gpio_write_pin(port, pin, GPIO_PIN_RESET);
}

void blfm_led_toggle(GPIO_TypeDef *port, uint16_t pin) {
    if (blfm_gpio_read_pin(port, pin) == GPIO_PIN_SET)
        blfm_gpio_write_pin(port, pin, GPIO_PIN_RESET);
    else
        blfm_gpio_write_pin(port, pin, GPIO_PIN_SET);
}
