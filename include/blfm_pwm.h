#ifndef BLFM_PWM_H
#define BLFM_PWM_H

#include <stdint.h>

// Initialize PWM hardware with specified frequency (in Hz)
void blfm_pwm_init(uint32_t frequency);

// Set PWM duty cycle for a given channel (0-100%)
void blfm_pwm_set_duty_cycle(uint8_t channel, uint8_t duty_cycle_percent);

// Enable PWM output on specified channel
void blfm_pwm_enable(uint8_t channel);

// Disable PWM output on specified channel
void blfm_pwm_disable(uint8_t channel);

#endif // BLFM_PWM_H
