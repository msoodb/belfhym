#include "blfm_button.h"
#include "blfm_gpio.h"

void blfm_button_init(GPIO_TypeDef *port, uint16_t pin) {
    blfm_gpio_init_pin(port, pin, GPIO_INPUT_PULLUP);
}

bool blfm_button_is_pressed(GPIO_TypeDef *port, uint16_t pin) {
    return blfm_gpio_read_pin(port, pin) == GPIO_PIN_RESET;
}
