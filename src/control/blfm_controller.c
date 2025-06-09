
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_controller.h"
#include "FreeRTOS.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_types.h"
#include "libc_stubs.h"
#include "task.h"
#include <stdbool.h>

#define LCD_CYCLE_COUNT 50 // number of loops before switching mode
#define SWEEP_MIN_ANGLE 0
#define SWEEP_MAX_ANGLE 180

static int lcd_mode = 0;
static int lcd_counter = 0;
static bool direction = true;

typedef enum { STATE_FORWARD, STATE_BACKWARD, STATE_ROTATE } robot_state_t;
typedef enum { MOTOR_BACKWARD = 0, MOTOR_FORWARD = 1 } motor_direction_t;
static robot_state_t state = STATE_FORWARD;
//static TickType_t state_start_time = 0;
const TickType_t ROTATE_DURATION = pdMS_TO_TICKS(360); // rotate for 700ms
//static bool turn_left = true;

static void uint_to_str(char *buf, uint16_t value);

/*
  functions
 */
static void uint_to_str(char *buf, uint16_t value) {
  // max 3 digits (0-999)
  if (value >= 100) {
    buf[0] = '0' + (value / 100) % 10;
    buf[1] = '0' + (value / 10) % 10;
    buf[2] = '0' + value % 10;
    buf[3] = '\0';
  } else if (value >= 10) {
    buf[0] = '0' + (value / 10) % 10;
    buf[1] = '0' + value % 10;
    buf[2] = '\0';
  } else {
    buf[0] = '0' + value;
    buf[1] = '\0';
  }
}

void blfm_controller_init(void) {
  // Reserved for future initialization
}

// Fill 'out' based on 'in'
void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out) {
  if (!in || !out)
    return;

  switch (state) {
  case STATE_FORWARD:
    if (in->ultrasonic.distance_mm > 200) {
      blfm_gpio_set_pin((uint32_t)LED_DEBUG_PORT, LED_DEBUG_PIN);
      out->motor.left.direction = MOTOR_FORWARD;
      out->motor.right.direction = MOTOR_FORWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
    } else {
      state = STATE_BACKWARD;
    }
    break;

  case STATE_BACKWARD:
    if (in->ultrasonic.distance_mm < 250) {
      out->motor.left.direction = MOTOR_BACKWARD;
      out->motor.right.direction = MOTOR_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
    } else {
      state = STATE_ROTATE;
    }
    break;

  case STATE_ROTATE:
    blfm_gpio_clear_pin((uint32_t)LED_DEBUG_PORT, LED_DEBUG_PIN);
    out->motor.left.direction = MOTOR_FORWARD;
    out->motor.right.direction = MOTOR_BACKWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    state = STATE_FORWARD;
    break;
  }
 
  out->led.mode = BLFM_LED_MODE_BLINK;
  out->led.blink_speed_ms = 200; // in->ultrasonic.distance_mm;

  if (in->ultrasonic.distance_mm < 100) {
    out->alarm.active = true;
    out->alarm.pattern_id = 1;
    out->alarm.duration_ms = 500;
    out->alarm.volume = 10;
  } else {
    out->alarm.active = false;
  }

  if (direction)
    out->servo.angle += 5;
  else
    out->servo.angle -= 5;

  if (out->servo.angle >= SWEEP_MAX_ANGLE)
    direction = false;
  else if (out->servo.angle <= SWEEP_MIN_ANGLE)
    direction = true;

  // LCD display logic
  char buf1[17]; // 16 + null terminator
  char num_buf[12];

  lcd_counter++;
  if (lcd_counter >= LCD_CYCLE_COUNT) {
    lcd_counter = 0;
    lcd_mode = (lcd_mode + 1) % 3; // 0=dist,1=speed,2=temp
  }
  // lcd_mode = 2;

  if (lcd_mode == 0) {
    strcpy(buf1, "Dist: ");
    uint_to_str(num_buf, in->ultrasonic.distance_mm);
    strcat(buf1, num_buf);
    strcat(buf1, " mm");
  } else if (lcd_mode == 1) {
    strcpy(buf1, "Speed: ");
    // uint_to_str(num_buf, in->imu.speed_cm_s);
    strcat(buf1, num_buf);
    strcat(buf1, " cm/s");
  } else if (lcd_mode == 2) {
    strcpy(buf1, "Temp: ");
    uint_to_str(num_buf, in->temperature.temperature_mc);
    strcat(buf1, num_buf);
    strcat(buf1, " C");
  }
  // Copy to display command
  strcpy(out->display.line1, buf1);
  strcpy(out->display.line2, num_buf);
}

void blfm_controller_process_bigsound(const blfm_bigsound_event_t *event,
                                      blfm_actuator_command_t *out) {
  if (!event || !out)
    return;

  return;
  // Handle big sound event (e.g., alert display)
  strcpy(out->display.line1, "!!! ALERT !!!");
  strcpy(out->display.line2, "Noise detected");
  out->led.mode = BLFM_LED_MODE_ON;

  //   blfm_gpio_config_output((uint32_t)GPIOB, 11);
  blfm_gpio_set_pin((uint32_t)GPIOB, 11);

  // crude short delay (adjust if needed)
  for (volatile int i = 0; i < 10000; i++) {
    __asm__("nop");
  }

  blfm_gpio_clear_pin((uint32_t)GPIOB, 11);
}
