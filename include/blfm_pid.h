#ifndef BLFM_PID_H
#define BLFM_PID_H

#include <stdint.h>

typedef struct {
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int16_t setpoint;
    int16_t integral;
    int16_t prev_error;
    int16_t output;
} blfm_pid_t;

void blfm_pid_init(blfm_pid_t *pid, int16_t kp, int16_t ki, int16_t kd);
int16_t blfm_pid_compute(blfm_pid_t *pid, int16_t input);

#endif
