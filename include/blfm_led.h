#ifndef BLFM_LED_H
#define BLFM_LED_H

#include <stdint.h>

void blfm_led_init(void);
void blfm_led_set_blink_rate(uint16_t delay_ms);
void blfm_led_blink_task(void *params);

#endif
