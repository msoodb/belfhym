
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */
#ifndef BLFM_TYPES_H
#define BLFM_TYPES_H

#include <stdint.h>

/**
 * @brief IMU sensor raw data (accelerometer + gyroscope)
 */
typedef struct {
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} blfm_imu_data_t;

/**
 * @brief Ultrasonic sensor data (distance in mm)
 */
typedef struct {
    uint16_t distance_mm;
} blfm_ultrasonic_data_t;

/**
 * @brief Motor control command (speed and direction)
 * Speed range 0-255 (PWM), direction 0=stop,1=forward,2=reverse
 */
typedef struct {
    uint8_t speed;
    uint8_t direction;
} blfm_motor_command_t;

/**
 * @brief Alarm state
 */
typedef enum {
    ALARM_OFF = 0,
    ALARM_ON  = 1,
} blfm_alarm_state_t;

/**
 * @brief Radio message structure (placeholder)
 */
typedef struct {
    uint8_t data[64];
    uint8_t length;
} blfm_radio_message_t;

#endif // BLFM_TYPES_H
