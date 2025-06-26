
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
#define MODE_BUTTON_DEBOUNCE_MS 100
#define IR_CONTROL_TIMEOUT_MS 200

static int pseudo_random(int min, int max) {
  static uint32_t seed = 123456789;
  seed = seed * 1664525 + 1013904223;
  return min + (seed % (max - min + 1));
}

static int backward_ticks = 0;
static int rotate_ticks = 0;
static int rotate_duration = 0;

static blfm_system_state_t blfm_system_state = {
    .current_mode = BLFM_MODE_MANUAL, .motion_state = BLFM_MOTION_STOP};

// static void uint_to_str(char *buf, uint16_t value);

/*
  functions
 */
/*static void uint_to_str(char *buf, uint16_t value) {
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
  }*/

void blfm_controller_init(void) {
  // Reserved for future initialization
}

void blfm_controller_cycle_mode(blfm_actuator_command_t *out) {
  // Cycle through modes
  switch (blfm_system_state.current_mode) {
  case BLFM_MODE_AUTO:
    blfm_system_state.current_mode = BLFM_MODE_MANUAL;
    out->led.mode = BLFM_LED_MODE_BLINK;
    out->led.blink_speed_ms = 100;
    break;

  case BLFM_MODE_MANUAL:
    blfm_system_state.current_mode = BLFM_MODE_AUTO;
    out->led.mode = BLFM_LED_MODE_BLINK;
    out->led.blink_speed_ms = 500;
    break;

  default:
    blfm_system_state.current_mode = BLFM_MODE_EMERGENCY;
    out->led.mode = BLFM_LED_MODE_ON;
    out->led.blink_speed_ms = 0;
    break;
  }
  out->motor.left.direction = BLFM_MOTION_FORWARD;
  out->motor.right.direction = BLFM_MOTION_FORWARD;
  out->motor.left.speed = 0;
  out->motor.right.speed = 0;
  blfm_system_state.motion_state = BLFM_MOTION_STOP;
}

// Fill 'out' based on 'in'
void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out) {
  if (!in || !out)
    return;

  if (blfm_system_state.current_mode == BLFM_MODE_AUTO) {
    switch (blfm_system_state.motion_state) {
    case BLFM_MOTION_STOP:
    case BLFM_MOTION_FORWARD:
      if (in->ultrasonic.distance_mm <= ULTRASONIC_FORWARD_THRESH) {
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
        rotate_ticks = 0;
        rotate_duration = pseudo_random(MIN_ROTATE_TICKS, MAX_ROTATE_TICKS);
        blfm_system_state.motion_state = BLFM_MOTION_ROTATE_LEFT;
      }
      break;

    case BLFM_MOTION_ROTATE_LEFT:
      out->motor.left.direction = BLFM_MOTION_FORWARD;
      out->motor.right.direction = BLFM_MOTION_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;

      rotate_ticks++;
      if (rotate_ticks >= rotate_duration) {
        blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
      }
      break;

    default:
      out->motor.left.speed = 0;
      out->motor.right.speed = 0;
      break;
    }
  }

  out->led.mode = BLFM_LED_MODE_BLINK;
  out->led.blink_speed_ms = 100;

  // blfm_gpio_toggle_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
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
  if (!in || !out) {
    return;
  }

  blfm_ir_command_t command = in->command;
  if (blfm_system_state.current_mode != BLFM_MODE_MANUAL) {
    return;
  } 
  
  switch (command) {
    case BLFM_IR_CMD_UP:
      blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
      break;

    case BLFM_IR_CMD_DOWN:
      blfm_system_state.motion_state = BLFM_MOTION_BACKWARD;
      break;

    case BLFM_IR_CMD_LEFT:
      blfm_system_state.motion_state = BLFM_MOTION_ROTATE_LEFT;
      break;

    case BLFM_IR_CMD_RIGHT:
      blfm_system_state.motion_state = BLFM_MOTION_ROTATE_RIGHT;
      break;

    case BLFM_IR_CMD_NONE:
    default:
      blfm_system_state.motion_state = BLFM_MOTION_STOP;
      break;
  }

  // STEP 5: Output the motor command
  switch (blfm_system_state.motion_state) {
    case BLFM_MOTION_FORWARD:
      out->motor.left.direction = BLFM_MOTION_FORWARD;
      out->motor.right.direction = BLFM_MOTION_FORWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
      break;

    case BLFM_MOTION_BACKWARD:
      out->motor.left.direction = BLFM_MOTION_BACKWARD;
      out->motor.right.direction = BLFM_MOTION_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
      break;

    case BLFM_MOTION_ROTATE_LEFT:
      out->motor.left.direction = BLFM_MOTION_BACKWARD;
      out->motor.right.direction = BLFM_MOTION_FORWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
      break;

    case BLFM_MOTION_ROTATE_RIGHT:
      out->motor.left.direction = BLFM_MOTION_FORWARD;
      out->motor.right.direction = BLFM_MOTION_BACKWARD;
      out->motor.left.speed = 255;
      out->motor.right.speed = 255;
      break;

    case BLFM_MOTION_STOP:
    default:
      out->motor.left.direction = BLFM_MOTION_FORWARD;
      out->motor.right.direction = BLFM_MOTION_FORWARD;
      out->motor.left.speed = 0;
      out->motor.right.speed = 0;
      break;
  }
}

void blfm_controller_process_joystick(const blfm_joystick_event_t *evt,
                                      blfm_actuator_command_t *out) {
  if (!evt || !out) {
    return;
  }

  /*if (evt->event_type == BLFM_JOYSTICK_EVENT_PRESSED) {
    blfm_controller_cycle_mode(out);
    }*/

  if (blfm_system_state.current_mode == BLFM_MODE_MANUAL) {
    switch (evt->direction) {
    case BLFM_JOYSTICK_DIR_UP:
      blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
      break;

    case BLFM_JOYSTICK_DIR_DOWN:
      blfm_system_state.motion_state = BLFM_MOTION_BACKWARD;
      break;

    case BLFM_JOYSTICK_DIR_LEFT:
      blfm_system_state.motion_state = BLFM_MOTION_ROTATE_LEFT;
      break;

    case BLFM_JOYSTICK_DIR_RIGHT:
      blfm_system_state.motion_state = BLFM_MOTION_ROTATE_RIGHT;
      break;

    default:
      blfm_system_state.motion_state = BLFM_MOTION_STOP;
      break;
    }
  }

  // Output based on motion state
  switch (blfm_system_state.motion_state) {
  case BLFM_MOTION_FORWARD:
    out->motor.left.direction = BLFM_MOTION_FORWARD;
    out->motor.right.direction = BLFM_MOTION_FORWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    break;

  case BLFM_MOTION_BACKWARD:
    out->motor.left.direction = BLFM_MOTION_BACKWARD;
    out->motor.right.direction = BLFM_MOTION_BACKWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    break;

  case BLFM_MOTION_ROTATE_LEFT:
    out->motor.left.direction = BLFM_MOTION_BACKWARD;
    out->motor.right.direction = BLFM_MOTION_FORWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    break;

  case BLFM_MOTION_ROTATE_RIGHT:
    out->motor.left.direction = BLFM_MOTION_FORWARD;
    out->motor.right.direction = BLFM_MOTION_BACKWARD;
    out->motor.left.speed = 255;
    out->motor.right.speed = 255;
    break;

  case BLFM_MOTION_STOP:
  default:
    out->motor.left.direction = BLFM_MOTION_FORWARD;
    out->motor.right.direction = BLFM_MOTION_BACKWARD;
    out->motor.left.speed = 0;
    out->motor.right.speed = 0;
    break;
  }
}

void blfm_controller_process_joystick_click(const blfm_joystick_event_t *event,
                                            blfm_actuator_command_t *command) {
  if (!event || !command) {
    return;
  }
  // blfm_controller_cycle_mode(command);
}

void blfm_controller_process_mode_button(const blfm_mode_button_event_t *event,
                                         blfm_actuator_command_t *command) {
  (void) command;
  static uint32_t last_press_tick = 0;

  if (event && event->event_type == BLFM_MODE_BUTTON_EVENT_PRESSED) {
    uint32_t now = xTaskGetTickCount();
    uint32_t diff = now - last_press_tick;

    if (diff > pdMS_TO_TICKS(MODE_BUTTON_DEBOUNCE_MS)) {
      // blfm_controller_cycle_mode(command);
      blfm_gpio_toggle_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
      last_press_tick = now;
    }
  }
}

bool blfm_controller_check_ir_timeout(blfm_actuator_command_t *out) {
  (void) out;
  return false;
}

