
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


#define BLFM_OLED_WIDTH 128
#define BLFM_OLED_HEIGHT 32
#define BLFM_OLED_PAGES (BLFM_OLED_HEIGHT / 8)
#define BLFM_OLED_MAX_SMALL_TEXT_LEN 12
#define BLFM_OLED_MAX_BIG_TEXT_LEN 16

//-----------------------
//  sensors
//-----------------------

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
  uint16_t distance_mm;
} blfm_ultrasonic_data_t;



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
  uint16_t x;  // ADC value from X-axis
  uint16_t y;  // ADC value from Y-axis
  bool pressed; // true if button is pressed
} blfm_joystick_data_t;

// Directional interpretation of the joystick position
typedef enum {
  BLFM_JOYSTICK_DIR_NONE = 0,
  BLFM_JOYSTICK_DIR_UP,
  BLFM_JOYSTICK_DIR_DOWN,
  BLFM_JOYSTICK_DIR_LEFT,
  BLFM_JOYSTICK_DIR_RIGHT
} blfm_joystick_direction_t;

// Type of joystick event (pressed/released)
typedef enum {
  BLFM_JOYSTICK_EVENT_NONE = 0,
  BLFM_JOYSTICK_EVENT_PRESSED,
  BLFM_JOYSTICK_EVENT_RELEASED
} blfm_joystick_event_type_t;

// Full interpreted joystick event
typedef struct {
  uint32_t timestamp;                        // Optional timestamp (e.g., from xTaskGetTickCount())
  blfm_joystick_event_type_t event_type;     // Button event type
  blfm_joystick_direction_t direction;       // Directional interpretation
} blfm_joystick_event_t;

typedef struct {
  blfm_ultrasonic_data_t ultrasonic;
  blfm_joystick_data_t joystick;
  blfm_joystick_event_t joystick_event;     // Interpreted direction + event
} blfm_sensor_data_t;

// Raw ADC data from the joystick (X and Y axis)



//-----------------------
//  actuators
//-----------------------
typedef struct {
  uint16_t speed;     // 0–255
  uint8_t direction; // 0 = forward, 1 = backward
} blfm_single_motor_command_t;

typedef struct {
  blfm_single_motor_command_t left;
  blfm_single_motor_command_t right;
} blfm_motor_command_t;


typedef struct {
  uint8_t angle;           // Servo angle in degrees (0-180)
  uint16_t pulse_width_us; // Optional: pulse width in microseconds (e.g.,
                           // 1000-2000us)
} blfm_servomotor_command_t;

typedef enum {
  BLFM_SERVOMOTOR_NECK = 0,
  BLFM_SERVOMOTOR_COUNT
} blfm_servomotor_id_t;



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
  blfm_motor_command_t motor;
  blfm_oled_command_t oled;
  blfm_led_command_t led;
  blfm_servomotor_command_t servo;
} blfm_actuator_command_t;

#endif // BLFM_TYPES_H
