/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_INTERRUPTS_H
#define BLFM_INTERRUPTS_H

#include <stdint.h>

typedef void (*blfm_exti_callback_t)(void);

typedef enum {
    BLFM_EXTI_TRIGGER_RISING = 0x01,
    BLFM_EXTI_TRIGGER_FALLING = 0x02,
    BLFM_EXTI_TRIGGER_BOTH = 0x03
} blfm_exti_trigger_t;

void blfm_exti_init(uint8_t gpio_port, uint8_t pin, blfm_exti_trigger_t trigger, blfm_exti_callback_t callback);
void blfm_exti_register_callback(uint8_t exti_line, blfm_exti_callback_t callback);

#endif /* BLFM_INTERRUPTS_H */