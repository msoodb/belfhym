#include "blfm_alarm.h"
#include "blfm_gpio.h"

void blfm_alarm_init(void) {
    // Initialize GPIO pin connected to the alarm (buzzer/LED/etc)
    blfm_gpio_init_alarm_pin();
    blfm_alarm_set(0);  // ensure alarm is off initially
}

void blfm_alarm_set(uint8_t state) {
  /*if (state) {
        blfm_gpio_set_alarm_pin_high();
    } else {
        blfm_gpio_set_alarm_pin_low();
	}*/
}

void blfm_alarm_on(void) {}

void blfm_alarm_off(void) {}

