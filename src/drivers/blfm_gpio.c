#include "blfm_gpio.h"
#include "stm32f1xx.h"

#define ALARM_GPIO_PORT GPIOB
#define ALARM_GPIO_PIN (1 << 5) // Pin 5 mask

void blfm_gpio_init_pin(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode) {
    // Enable port clock
    if (port == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (port == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (port == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    uint32_t pos = (pin % 8) * 4;
    volatile uint32_t *conf = (pin < 8) ? &port->CRL : &port->CRH;

    *conf &= ~(0xF << pos);
    *conf |= (mode << pos);
}

void blfm_gpio_write_pin(GPIO_TypeDef *port, uint16_t pin, GPIOPinState state) {
    if (state == GPIO_PIN_SET)
        port->BSRR = (1 << pin);
    else
        port->BRR = (1 << pin);
}

GPIOPinState blfm_gpio_read_pin(GPIO_TypeDef *port, uint16_t pin) {
    return (port->IDR & (1 << pin)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void blfm_gpio_init_alarm_pin(void) {
    // Enable GPIOB clock (RCC)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Configure pin as General purpose output push-pull, max speed 2 MHz
    ALARM_GPIO_PORT->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);  // Clear mode and config
    ALARM_GPIO_PORT->CRL |= (GPIO_CRL_MODE5_1);  // Output mode, max speed 2 MHz
    ALARM_GPIO_PORT->CRL &= ~GPIO_CRL_CNF5;     // General purpose output push-pull

    // Set initial output low (alarm off)
    ALARM_GPIO_PORT->BRR = ALARM_GPIO_PIN;
}

void blfm_gpio_set_alarm_pin_high(void) {
  ALARM_GPIO_PORT->BSRR = ALARM_GPIO_PIN;  // Set pin high atomically
}

void blfm_gpio_set_alarm_pin_low(void){
  GPIOB->BSRR = GPIO_BSRR_BR0;
}
