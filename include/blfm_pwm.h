#ifndef BLFM_PWM_H
#define BLFM_PWM_H

#include "stm32f1xx.h"

typedef struct {
    TIM_TypeDef *timer;
    uint32_t channel;
    uint32_t pwm_pin;
    uint16_t max_duty_cycle; // e.g., 1000 for 10-bit resolution
} blfm_pwm_t;

void blfm_pwm_init(blfm_pwm_t *pwm, uint16_t frequency);
void blfm_pwm_start(blfm_pwm_t *pwm);
void blfm_pwm_stop(blfm_pwm_t *pwm);
void blfm_pwm_set_duty_cycle(blfm_pwm_t *pwm, uint16_t duty_cycle);

#endif
