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

#include <stdbool.h>
#include <stdint.h>

//==============================================================================
// DISPLAY CONSTANTS
//==============================================================================

#define BLFM_DISPLAY_LINE_LENGTH 17 // 16 chars + '\0'

#define BLFM_OLED_WIDTH 128
#define BLFM_OLED_HEIGHT 32
#define BLFM_OLED_PAGES (BLFM_OLED_HEIGHT / 8)
#define BLFM_OLED_MAX_SMALL_TEXT_LEN 12
#define BLFM_OLED_MAX_BIG_TEXT_LEN 16

//==============================================================================
// COMMUNICATION - ESP32
//==============================================================================

typedef enum {
  BLFM_ESP32_CMD_NONE = 0,
  BLFM_ESP32_CMD_UP,
  BLFM_ESP32_CMD_DOWN,
  BLFM_ESP32_CMD_LEFT,
  BLFM_ESP32_CMD_RIGHT,
  BLFM_ESP32_CMD_STOP
} blfm_esp32_command_type_t;

typedef struct {
  blfm_esp32_command_type_t command;
  uint8_t speed;         // 0-255, optional: speed level from gamepad
  uint32_t timestamp;    // Tick count when received
} blfm_esp32_event_t;

//==============================================================================
// SENSORS
//==============================================================================

// Event definitions
typedef enum {
  BLFM_MODE_BUTTON_EVENT_PRESSED,
  BLFM_MODE_BUTTON_EVENT_RELEASED
} blfm_mode_button_event_type_t;

typedef struct {
  blfm_mode_button_event_type_t event_type;
  uint32_t timestamp;
} blfm_mode_button_event_t;

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
  uint16_t raw_value;  // raw ADC reading from potentiometer (0-4095)
} blfm_potentiometer_data_t;

typedef struct {
  int32_t temperature_mc;
} blfm_temperature_data_t;

typedef enum {
  BLFM_IR_CMD_NONE = 0,
  BLFM_IR_CMD_1 = 0x45,
  BLFM_IR_CMD_2 = 0x46,
  BLFM_IR_CMD_3 = 0x47,
  BLFM_IR_CMD_4 = 0x44,
  BLFM_IR_CMD_5 = 0x40,
  BLFM_IR_CMD_6 = 0x43,
  BLFM_IR_CMD_7 = 0x07,
  BLFM_IR_CMD_8 = 0x15,
  BLFM_IR_CMD_9 = 0x09,
  BLFM_IR_CMD_0 = 0x19,
  BLFM_IR_CMD_STAR = 0x16,
  BLFM_IR_CMD_HASH = 0x0D,
  BLFM_IR_CMD_UP = 0x18,
  BLFM_IR_CMD_DOWN = 0x52,
  BLFM_IR_CMD_LEFT = 0x08,
  BLFM_IR_CMD_RIGHT = 0x5A,
  BLFM_IR_CMD_OK = 0x1C,
  BLFM_IR_CMD_REPEAT = 0xFFFFFFFF
} blfm_ir_command_t;

typedef struct {
  uint32_t timestamp;        // Tick count at event
  uint32_t pulse_us;         // Raw IR code received
  blfm_ir_command_t command; // Decoded command
} blfm_ir_remote_event_t;


typedef struct {
  blfm_ultrasonic_data_t ultrasonic;
  blfm_imu_data_t imu;
  blfm_temperature_data_t temperature;
  blfm_potentiometer_data_t potentiometer;
} blfm_sensor_data_t;

//==============================================================================
// BIGSOUND
//==============================================================================

typedef enum { BIGSOUND_EVENT_DETECTED = 1 } blfm_bigsound_event_type_t;

typedef struct {
  uint32_t timestamp;                    // Tick count when event happened
  blfm_bigsound_event_type_t event_type; // Type of bigsound event
} blfm_bigsound_event_t;

//==============================================================================
// ACTUATORS
//==============================================================================

typedef struct {
  uint16_t speed;     // 0–255
  uint8_t direction; // 0 = forward, 1 = backward
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

typedef enum {
  BLFM_SERVO_TYPE_SCANNER = 0,     // Continuous scanning motion
  BLFM_SERVO_TYPE_TRACKER,         // Position tracking based on sensors
  BLFM_SERVO_TYPE_MANUAL,          // Manual control only
  BLFM_SERVO_TYPE_STATIC,          // Fixed position
  BLFM_SERVO_TYPE_PROPORTIONAL,    // Precise proportional positioning
  BLFM_SERVO_TYPE_RADAR            // Radar sweep from extreme left to right
} blfm_servo_type_t;

typedef struct {
  uint8_t angle;                    // Target angle (0-180)
  uint16_t pulse_width_us;          // Optional: direct pulse width (1000-2000us)
  
  // Scanner-specific parameters
  uint8_t scan_min_angle;           // Scanner minimum angle (default: 0)
  uint8_t scan_max_angle;           // Scanner maximum angle (default: 180)
  uint8_t scan_step;                // Scanner step size (default: 5)
  uint16_t scan_delay_ms;           // Scanner delay between steps (default: 100)
  
  // Tracker-specific parameters
  int16_t target_x;                 // Target X coordinate for tracking
  int16_t target_y;                 // Target Y coordinate for tracking
  uint8_t tracking_speed;           // Tracking movement speed (1-10)
  
  // Proportional-specific parameters
  int16_t proportional_input;       // Input value (-1000 to +1000)
  uint8_t deadband;                 // Deadband around center (0-50)
  uint8_t travel_limit;             // Travel limit percentage (50-100)
  
  // General parameters
  uint8_t speed;                    // Movement speed (1-10, 10=fastest)
  bool enable_smooth;               // Enable smooth movement
  bool reverse_direction;           // Reverse servo direction
} blfm_servomotor_command_t;

typedef struct {
  char line1[BLFM_DISPLAY_LINE_LENGTH];
  char line2[BLFM_DISPLAY_LINE_LENGTH];
} blfm_display_command_t;

typedef enum {
  BLFM_OLED_ICON_NONE = 0,
  BLFM_OLED_ICON_HEART,
  BLFM_OLED_ICON_SMILEY,
  BLFM_OLED_ICON_STAR,
} blfm_oled_icon_t;

typedef struct {
  blfm_oled_icon_t icon1;  // top-left icon
  blfm_oled_icon_t icon2;  // top-left icon (next to icon1)
  blfm_oled_icon_t icon3;  // top-right icon
  blfm_oled_icon_t icon4;  // top-right icon (next to icon3)

  char smalltext1[BLFM_OLED_MAX_SMALL_TEXT_LEN];
  char bigtext[BLFM_OLED_MAX_BIG_TEXT_LEN];
  char smalltext2[BLFM_OLED_MAX_SMALL_TEXT_LEN];

  uint8_t invert;          // invert display
  uint8_t progress_percent; // optional progress bar

} blfm_oled_command_t;

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
  blfm_led_mode_t mode;    // off, on, or blink
  uint16_t blink_speed_ms; // blinking speed in milliseconds (period)
  uint8_t pattern_id; // blink pattern id (e.g., 0=solid, 1=short pulses, 2=SOS,
                      // etc)
  uint8_t
      brightness; // brightness level (0-255), optional if hardware supports PWM
} blfm_led_command_t;

typedef struct {
  uint8_t data[64];
  uint8_t length;
} blfm_radio_command_t;

typedef struct {
  blfm_motor_command_t motor;
  blfm_display_command_t display;
  blfm_oled_command_t oled;
  blfm_led_command_t led;
  blfm_alarm_command_t alarm;
  blfm_radio_command_t radio;
  blfm_servomotor_command_t servo1;
  blfm_servomotor_command_t servo2;
  blfm_servomotor_command_t servo3;
  blfm_servomotor_command_t servo4;
  blfm_stepmotor_command_t stepmotor;
} blfm_actuator_command_t;

#endif // BLFM_TYPES_H
