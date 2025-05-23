
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
#include <stdbool.h>

#define BLFM_DISPLAY_LINE_LENGTH 17 // 16 chars + '\0'

typedef struct {
  int16_t acc_x;
  int16_t acc_y;
  int16_t acc_z;
  int16_t gyro_x;
  int16_t gyro_y;
  int16_t gyro_z;
} blfm_imu_data_t;

typedef struct {
  uint16_t distance_mm;
} blfm_ultrasonic_data_t;

typedef struct {
  int32_t temperature_mc;
} blfm_temperature_data_t;

typedef struct {
  blfm_ultrasonic_data_t ultrasonic;
  blfm_imu_data_t imu;
  blfm_temperature_data_t temperature;
} blfm_sensor_data_t;

typedef struct {
  uint8_t speed;
  uint8_t direction;
} blfm_motor_command_t;

typedef struct {
  char line1[BLFM_DISPLAY_LINE_LENGTH];
  char line2[BLFM_DISPLAY_LINE_LENGTH];
} blfm_display_command_t;

typedef struct {
  bool active;
  uint8_t pattern_id;
  uint16_t duration_ms;
  uint8_t volume;
} blfm_alarm_command_t;

typedef enum {
  BLFM_LED_MODE_OFF = 0,
  BLFM_LED_MODE_ON,
  BLFM_LED_MODE_BLINK,
} blfm_led_mode_t;

typedef struct {
  blfm_led_mode_t mode;     // off, on, or blink
  uint16_t blink_speed_ms;  // blinking speed in milliseconds (period)
  uint8_t pattern_id;       // blink pattern id (e.g., 0=solid, 1=short pulses, 2=SOS, etc)
  uint8_t brightness;       // brightness level (0-255), optional if hardware supports PWM
} blfm_led_command_t;

typedef struct {
  uint8_t data[64];
  uint8_t length;
} blfm_radio_command_t;

typedef struct {
  blfm_motor_command_t motor;
  blfm_display_command_t display;
  blfm_led_command_t led;
  blfm_alarm_command_t alarm;
  blfm_radio_command_t radio;
} blfm_actuator_command_t;

#endif // BLFM_TYPES_H
