
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


//-----------------------
//  sensors
//-----------------------

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


//-----------------------
//  bigsound
//-----------------------
typedef enum {
  BIGSOUND_EVENT_DETECTED = 1
} blfm_bigsound_event_type_t;

typedef struct {
  uint32_t timestamp;                    // Tick count when event happened
  blfm_bigsound_event_type_t event_type; // Type of bigsound event
} blfm_bigsound_event_t;




//-----------------------
//  actuators
//-----------------------
typedef struct {
  uint8_t speed;      // 0–255
  uint8_t direction;  // 0 = forward, 1 = backward
} blfm_single_motor_command_t;

typedef struct {
  blfm_single_motor_command_t left;
  blfm_single_motor_command_t right;
} blfm_motor_command_t;

typedef struct {
  int32_t target_position; // e.g., in steps
  uint16_t speed;          // steps per second
  bool direction;          // false = CW, true = CCW, or use enum
  bool enabled;            // whether to drive this motor or ignore it
} blfm_stepmotor_command_t;

typedef enum {
  BLFM_STEPMOTOR_NECK = 0,
  BLFM_STEPMOTOR_ELBOW,
  BLFM_STEPMOTOR_WRIST,
  BLFM_STEPMOTOR_COUNT
} blfm_stepmotor_id_t;

typedef struct {
  uint8_t angle;    // Servo angle in degrees (0-180)
  uint16_t pulse_width_us; // Optional: pulse width in microseconds (e.g., 1000-2000us)
} blfm_servomotor_command_t;

typedef enum {
  BLFM_SERVOMOTOR_NECK = 0,
  BLFM_SERVOMOTOR_COUNT
} blfm_servomotor_id_t;

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
  blfm_servomotor_command_t servo;
  blfm_stepmotor_command_t stepmotor;
} blfm_actuator_command_t;

#endif // BLFM_TYPES_H
