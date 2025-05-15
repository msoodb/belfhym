#ifndef BLFM_MOTOR_H
#define BLFM_MOTOR_H

#include <stdint.h>

void blfm_motor_init(void);

// Set motor speed and direction
// speed: from 0 (stop) to max speed (e.g., 100)
// direction: -1 for reverse, 1 for forward, 0 stop
void blfm_motor_set_speed_direction(int16_t speed, int16_t direction);

#endif // BLFM_MOTOR_H
