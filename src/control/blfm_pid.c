
#include "blfm_pid.h"

void blfm_pid_init(blfm_pid_t *pid, int16_t kp, int16_t ki, int16_t kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->setpoint = 0;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->output = 0;
}

// You can scale gains by a factor of 10 or 100 to simulate decimals
// e.g., kp=150 means 1.5 if using scale factor of 100
int16_t blfm_pid_compute(blfm_pid_t *pid, int16_t input) {
    int16_t error = pid->setpoint - input;
    pid->integral += error;
    int16_t derivative = error - pid->prev_error;

    // Simple fixed-point scaling
    int32_t output = ((int32_t)pid->kp * error +
                      (int32_t)pid->ki * pid->integral +
                      (int32_t)pid->kd * derivative) / 100; // scaling factor

    pid->prev_error = error;
    pid->output = (int16_t)output;
    return pid->output;
}
