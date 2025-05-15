
#include "blfm_pwm.h"

#define GPIO_PIN_8 (1 << 8)

void blfm_pwm_init(blfm_pwm_t *pwm, uint16_t frequency) {
    // Enable clock for the timer and GPIO pin (Assuming Timer1 for this example)
    if (pwm->timer == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  // Enable TIM1 clock
    }

    // Configure GPIO pin as alternate function for PWM (Assuming PA8 for TIM1 CH1)
    if (pwm->pwm_pin == GPIO_PIN_8) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // Enable GPIOA clock
        GPIOA->CRH &= ~(0xF << 4);           // Reset pin configuration
        GPIOA->CRH |= (0xA << 4);            // Alternate function push-pull (AF1)
    }

    // Configure timer frequency and prescaler
    uint32_t timer_freq = SystemCoreClock / (pwm->max_duty_cycle + 1);
    uint16_t prescaler = (timer_freq / frequency) - 1;
    pwm->timer->PSC = prescaler;            // Set prescaler
    pwm->timer->ARR = pwm->max_duty_cycle;  // Auto-reload value (max duty cycle)

    // Configure PWM channel (Assuming TIM1 CH1)
    pwm->timer->CCR1 = 0;   // Initial duty cycle = 0
    pwm->timer->CCMR1 |= (0x6 << 4); // PWM mode 1 (active when counter < CCR1)
    pwm->timer->CCER |= TIM_CCER_CC1E;  // Enable channel 1 output

    pwm->timer->CR1 |= TIM_CR1_CEN;     // Enable timer
}

void blfm_pwm_start(blfm_pwm_t *pwm) {
    pwm->timer->CR1 |= TIM_CR1_CEN;     // Start the timer
}

void blfm_pwm_stop(blfm_pwm_t *pwm) {
    pwm->timer->CR1 &= ~TIM_CR1_CEN;    // Stop the timer
}

void blfm_pwm_set_duty_cycle(blfm_pwm_t *pwm, uint16_t duty_cycle) {
    if (duty_cycle <= pwm->max_duty_cycle) {
        pwm->timer->CCR1 = duty_cycle;  // Set duty cycle
    }
}
