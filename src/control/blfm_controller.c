
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
#include "blfm_state.h"
#include "blfm_types.h"
#include "libc_stubs.h"
#include "task.h"
#include <stdbool.h>

#define LCD_CYCLE_COUNT 50
#define SWEEP_MIN_ANGLE 0
#define SWEEP_MAX_ANGLE 180

// static int lcd_mode = 0;
// static int lcd_counter = 0;
// static bool direction = true;

#define ULTRASONIC_FORWARD_THRESH 20
#define BACKWARD_TICKS_MAX 2
#define MIN_ROTATE_TICKS 4
#define MAX_ROTATE_TICKS 6

static int pseudo_random(int min, int max) {
  static uint32_t seed = 123456789;
  seed = seed * 1664525 + 1013904223;
  return min + (seed % (max - min + 1));
}

static int backward_ticks = 0;
static int rotate_ticks = 0;
static int rotate_duration = 0;

static blfm_system_state_t blfm_system_state = {
    .current_mode = BLFM_MODE_AUTO, .motion_state = BLFM_MOTION_FORWARD};

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

  // blfm_gpio_toggle_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
  // blfm_gpio_clear_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);

  if (blfm_system_state.current_mode == BLFM_MODE_AUTO) {
    switch (blfm_system_state.motion_state) {
    case BLFM_MOTION_FORWARD:
      if (in->ultrasonic.distance_mm <= ULTRASONIC_FORWARD_THRESH) {
        // Obstacle detected — begin BACKWARD
        backward_ticks = 0;
        blfm_system_state.motion_state = BLFM_MOTION_BACKWARD;
      } else {
        out->motor.left.direction = BLFM_MOTION_FORWARD;
        out->motor.right.direction = BLFM_MOTION_FORWARD;
        out->motor.left.speed = 255;
        out->motor.right.speed = 255;
      }
      break;

    case BLFM_MOTION_BACKWARD:
      out->motor.left.direction = BLFM_MOTION_BACKWARD;
      out->motor.right.direction = BLFM_MOTION_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;

      backward_ticks++;
      if (backward_ticks >= BACKWARD_TICKS_MAX) {
        // Back up complete — begin ROTATE
        rotate_ticks = 0;
        rotate_duration = pseudo_random(MIN_ROTATE_TICKS, MAX_ROTATE_TICKS);
        blfm_system_state.motion_state = BLFM_MOTION_ROTATE;
      }
      break;

    case BLFM_MOTION_ROTATE:
      out->motor.left.direction = BLFM_MOTION_FORWARD;
      out->motor.right.direction = BLFM_MOTION_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;

      rotate_ticks++;
      if (rotate_ticks >= rotate_duration) {
        // Rotation complete — return to FORWARD
        blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
      }
      break;

    default:
      // Stop
      out->motor.left.speed = 0;
      out->motor.right.speed = 0;
      break;
    }
  }

  /*else if (blfm_system_state.current_mode == BLFM_MODE_MANUAL) {
    // You can add real manual logic here
    out->motor.left.direction = BLFM_MOTION_BACKWARD;
    out->motor.right.direction = BLFM_MOTION_BACKWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    }*/

  out->led.mode = BLFM_LED_MODE_BLINK;
  out->led.blink_speed_ms = in->ultrasonic.distance_mm; // 200;

  /* if (in->ultrasonic.distance_mm < 100) {
      out->alarm.active = true;
      out->alarm.pattern_id = 1;
      out->alarm.duration_ms = 500;
      out->alarm.volume = 10;
    } else {
      out->alarm.active = false;
      }*/

  /*if (direction)
    out->servo.angle += 5;
  else
    out->servo.angle -= 5;

  if (out->servo.angle >= SWEEP_MAX_ANGLE)
    direction = false;
  else if (out->servo.angle <= SWEEP_MIN_ANGLE)
    direction = true;
  */

  // LCD display logic
  /*char buf1[17]; // 16 + null terminator
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
  */
}

void blfm_controller_process_bigsound(const blfm_bigsound_event_t *event,
                                      blfm_actuator_command_t *out) {
  if (!event || !out)
    return;

  return;
  // Handle big sound event (e.g., alert display)
  strcpy(out->display.line1, "!!! ALERT !!!");
  strcpy(out->display.line2, "Noise detected");
  // out->led.mode = BLFM_LED_MODE_ON;

  //   blfm_gpio_config_output((uint32_t)GPIOB, 11);
  blfm_gpio_set_pin((uint32_t)GPIOB, 11);

  // crude short delay (adjust if needed)
  for (volatile int i = 0; i < 10000; i++) {
    __asm__("nop");
  }

  blfm_gpio_clear_pin((uint32_t)GPIOB, 11);
}

void blfm_controller_process_ir_remote(const blfm_ir_remote_event_t *in,
                                       blfm_actuator_command_t *out) {
  if (!in || !out)
    return;
}

void blfm_controller_process_joystick(const blfm_joystick_event_t *in,
                                      blfm_actuator_command_t *out) {
  return;
}

void blfm_system_state_reset(void) {
  blfm_system_state.current_mode = BLFM_MODE_AUTO;
  blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
}
