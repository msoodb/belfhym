
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_LED_H
#define BLFM_LED_H

#include "blfm_types.h"

void blfm_led_init(void);

void blfm_led_onboard_on(void);
void blfm_led_onboard_off(void);

void blfm_led_external_on(void);
void blfm_led_external_off(void);

void blfm_led_debug_on(void);
void blfm_led_debug_off(void);

void blfm_led_apply(const blfm_led_command_t *cmd);

#endif /* BLFM_LED_H */
