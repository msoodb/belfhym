#ifndef BLFM_ALARM_H
#define BLFM_ALARM_H

#include <stdint.h>

void blfm_alarm_init(void);

// Activate or deactivate the alarm
// state: 0 = off, 1 = on
void blfm_alarm_set(uint8_t state);

void blfm_alarm_on(void);
void blfm_alarm_off(void);

#endif // BLFM_ALARM_H
