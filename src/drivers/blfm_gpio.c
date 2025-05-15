
#include "blfm_gpio.h"

void blfm_gpio_init_output(blfm_gpio_pin_t gpio) {
    if (gpio.port == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (gpio.port == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (gpio.port == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    uint8_t pin_num = 0;
    for (uint16_t mask = 1; mask; mask <<= 1, pin_num++) {
        if (gpio.pin & mask) break;
    }

    volatile uint32_t *cr = (pin_num < 8) ? &gpio.port->CRL : &gpio.port->CRH;
    pin_num = pin_num % 8;

    *cr &= ~(0xF << (pin_num * 4));
    *cr |=  (0x1 << (pin_num * 4)); // Output, push-pull, max 10 MHz
}

void blfm_gpio_init_input(blfm_gpio_pin_t gpio) {
    if (gpio.port == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (gpio.port == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (gpio.port == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    uint8_t pin_num = 0;
    for (uint16_t mask = 1; mask; mask <<= 1, pin_num++) {
        if (gpio.pin & mask) break;
    }

    volatile uint32_t *cr = (pin_num < 8) ? &gpio.port->CRL : &gpio.port->CRH;
    pin_num = pin_num % 8;

    *cr &= ~(0xF << (pin_num * 4));
    *cr |=  (0x4 << (pin_num * 4)); // Input floating
}

void blfm_gpio_set(blfm_gpio_pin_t gpio) {
    gpio.port->BSRR = gpio.pin;
}

void blfm_gpio_clear(blfm_gpio_pin_t gpio) {
    gpio.port->BRR = gpio.pin;
}

void blfm_gpio_toggle(blfm_gpio_pin_t gpio) {
    gpio.port->ODR ^= gpio.pin;
}

int blfm_gpio_read(blfm_gpio_pin_t gpio) {
    return (gpio.port->IDR & gpio.pin) ? 1 : 0;
}
