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
#include "blfm_config.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_state.h"
#include "blfm_types.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define LCD_CYCLE_COUNT 50
#define SWEEP_MIN_ANGLE 0
#define SWEEP_MAX_ANGLE 180

#define ULTRASONIC_FORWARD_THRESH 20
#define MOTOR_BACKWARD_TICKS_MAX 2
#define MOTOR_MIN_ROTATE_TICKS 4
#define MOTOR_MAX_ROTATE_TICKS 6
#define MODE_BUTTON_DEBOUNCE_MS 100
#define IR_CONTROL_TIMEOUT_MS 200

#define MOTOR_DEFAULT_SPEED 255



#if BLFM_ENABLED_MOTOR && BLFM_ENABLED_ULTRASONIC
static int motor_backward_ticks = 0;
static int motor_rotate_ticks = 0;
static int motor_rotate_duration = 0;
#endif

char num_buf[12];

static blfm_system_state_t blfm_system_state = {
    .current_mode = BLFM_MODE_MANUAL, .motion_state = BLFM_MOTION_STOP};

/**
 * Helper: set motor motion from angle (-180 to 180) and speed.
 */
static void set_motor_motion_by_angle(int angle, int speed,
                                      blfm_motor_command_t *out);

/* -------------------- Utilities -------------------- */

#if BLFM_ENABLED_ULTRASONIC
static int pseudo_random(int min, int max) {
  static uint32_t seed = 123456789;
  seed = seed * 1664525 + 1013904223;
  return min + (seed % (max - min + 1));
}
#endif


/* -------------------- Motion Helpers -------------------- */
#if BLFM_ENABLED_IR_REMOTE
static int motion_state_to_angle(blfm_motion_state_t motion) {
  switch (motion) {
  case BLFM_MOTION_FORWARD:
  case BLFM_MOTION_STOP:
    return 0;
  case BLFM_MOTION_BACKWARD:
    return 180;
  case BLFM_MOTION_ROTATE_RIGHT:
    return 90;
  case BLFM_MOTION_ROTATE_LEFT:
    return -90;
  default:
    return 0;
  }
}
#endif /* BLFM_ENABLED_IR_REMOTE */

/**
 * Map angle (-180 to 180) and speed (0-255) to motor command.
 * Sets motor left/right direction and speed.
 *
 * No floating point used.
 */
static void set_motor_motion_by_angle(int angle, int speed,
                                      blfm_motor_command_t *out) {
  if (!out)
    return;

  // Normalize angle to -180..180
  while (angle > 180)
    angle -= 360;
  while (angle < -180)
    angle += 360;

  // Map angle to differential control
  // Example simple approach:
  // -90 = full left spin
  // +90 = full right spin
  // 0 = straight
  // ±180 = backward

  if (angle > 90) {
    // Hard right spin
    out->left.direction = BLFM_MOTION_BACKWARD;
    out->right.direction = BLFM_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle < -90) {
    // Hard left spin
    out->left.direction = BLFM_MOTION_BACKWARD;
    out->right.direction = BLFM_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle > 45) {
    // Right turn
    out->left.direction = BLFM_MOTION_FORWARD;
    out->right.direction = BLFM_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle < -45) {
    // Left turn
    out->left.direction = BLFM_MOTION_BACKWARD;
    out->right.direction = BLFM_MOTION_FORWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle > -45 && angle < 45) {
    // Forward
    out->left.direction = BLFM_MOTION_FORWARD;
    out->right.direction = BLFM_MOTION_FORWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else {
    // fallback stop
    out->left.direction = BLFM_MOTION_FORWARD;
    out->right.direction = BLFM_MOTION_FORWARD;
    out->left.speed = 0;
    out->right.speed = 0;
  }
}

/* -------------------- Public Controller -------------------- */

void blfm_controller_init(void) {
  // Reserved for future initialization
}

// --- Mode-setting functions ---
static void blfm_set_mode_auto(blfm_actuator_command_t *out) {
  blfm_system_state.current_mode = BLFM_MODE_AUTO;
  out->led.mode = BLFM_LED_MODE_BLINK;
  out->led.blink_speed_ms = 250;
}

static void blfm_set_mode_manual(blfm_actuator_command_t *out) {
  blfm_system_state.current_mode = BLFM_MODE_MANUAL;
  out->led.mode = BLFM_LED_MODE_OFF;
}

static void blfm_set_mode_emergency(blfm_actuator_command_t *out) {
  blfm_system_state.current_mode = BLFM_MODE_EMERGENCY;
  out->led.mode = BLFM_LED_MODE_ON;
}

// --- Public interface to change mode ---
void blfm_controller_change_mode(blfm_mode_t mode,
                                 blfm_actuator_command_t *out) {
  switch (mode) {
  case BLFM_MODE_AUTO:
    blfm_set_mode_auto(out);
    break;
  case BLFM_MODE_MANUAL:
    blfm_set_mode_manual(out);
    break;
  case BLFM_MODE_EMERGENCY:
  default:
    blfm_set_mode_emergency(out);
    break;
  }

  set_motor_motion_by_angle(0, 0, &out->motor);
  blfm_system_state.motion_state = BLFM_MOTION_STOP;
}

void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out) {
  if (!in || !out)
    return;

  //uint16_t led_blink_speed = 100;
  //blfm_led_mode_t led_mode = BLFM_LED_MODE_BLINK;

#if BLFM_ENABLED_ULTRASONIC
  if (blfm_system_state.current_mode == BLFM_MODE_AUTO) {
    switch (blfm_system_state.motion_state) {
    case BLFM_MOTION_STOP:
    case BLFM_MOTION_FORWARD:
      if (in->ultrasonic.distance_mm <= ULTRASONIC_FORWARD_THRESH) {
        motor_backward_ticks = 0;
        blfm_system_state.motion_state = BLFM_MOTION_BACKWARD;
      } else {
        set_motor_motion_by_angle(0, MOTOR_DEFAULT_SPEED, &out->motor);
      }
      break;

    case BLFM_MOTION_BACKWARD:
      set_motor_motion_by_angle(180, MOTOR_DEFAULT_SPEED, &out->motor);
      motor_backward_ticks++;
      if (motor_backward_ticks >= MOTOR_BACKWARD_TICKS_MAX) {
        motor_rotate_ticks = 0;
        motor_rotate_duration =
            pseudo_random(MOTOR_MIN_ROTATE_TICKS, MOTOR_MAX_ROTATE_TICKS);
        blfm_system_state.motion_state = BLFM_MOTION_ROTATE_LEFT;
      }
      break;

    case BLFM_MOTION_ROTATE_LEFT:
      set_motor_motion_by_angle(-90, MOTOR_DEFAULT_SPEED, &out->motor);
      motor_rotate_ticks++;
      if (motor_rotate_ticks >= motor_rotate_duration) {
        blfm_system_state.motion_state = BLFM_MOTION_FORWARD;
      }
      break;

    default:
      set_motor_motion_by_angle(0, 0, &out->motor);
      break;
    }
  }

#endif /* BLFM_ENABLED_ULTRASONIC */



#if BLFM_ENABLED_LED
  //out->led.mode = led_mode;
  //out->led.blink_speed_ms = led_blink_speed;
#endif

// Multiple servos - no default behavior needed, will be controlled by IR commands


#if BLFM_ENABLED_OLED
  out->oled.icon1 = BLFM_OLED_ICON_NONE;
  out->oled.icon2 = BLFM_OLED_ICON_NONE;
  out->oled.icon3 = BLFM_OLED_ICON_SMILEY;
  out->oled.icon4 = BLFM_OLED_ICON_NONE;

  safe_strncpy(out->oled.smalltext1, "Pass", BLFM_OLED_MAX_SMALL_TEXT_LEN);
  safe_strncpy(out->oled.bigtext, "BELFHYM 2025", BLFM_OLED_MAX_BIG_TEXT_LEN);
  safe_strncpy(out->oled.smalltext2, "V1", BLFM_OLED_MAX_SMALL_TEXT_LEN);

  out->oled.invert = 0;
  out->oled.progress_percent = 100;
#endif
}

#if BLFM_ENABLED_IR_REMOTE
void blfm_controller_process_ir_remote(const blfm_ir_remote_event_t *in,
                                       blfm_actuator_command_t *out) {
  if (!in || !out)
    return;

  int16_t speed = MOTOR_DEFAULT_SPEED;

  switch (in->command) {
  case BLFM_IR_CMD_1:
    blfm_controller_change_mode(BLFM_MODE_MANUAL, out);
    break;

  case BLFM_IR_CMD_2:
    blfm_controller_change_mode(BLFM_MODE_AUTO, out);
    break;

  case BLFM_IR_CMD_3:
    blfm_controller_change_mode(BLFM_MODE_EMERGENCY, out);
    break;

  // Servo1 control (PA0) - Keys 4, 5, 6
  case BLFM_IR_CMD_4:
    // Servo1 to minimum (-90°)
    out->servo1.proportional_input = -1000;
    break;
  case BLFM_IR_CMD_5:
    // Servo1 to center (0°)
    out->servo1.proportional_input = 0;
    break;
  case BLFM_IR_CMD_6:
    // Servo1 to maximum (+90°)
    out->servo1.proportional_input = 1000;
    break;
    
  // Servo2 control (PA1) - Keys 7, 8, 9
  case BLFM_IR_CMD_7:
    // Servo2 to minimum (-90°)
    out->servo2.proportional_input = -1000;
    break;
  case BLFM_IR_CMD_8:
    // Servo2 to center (0°)
    out->servo2.proportional_input = 0;
    break;
  case BLFM_IR_CMD_9:
    // Servo2 to maximum (+90°)
    out->servo2.proportional_input = 1000;
    break;
    
  // All servos control - Keys *, 0, #
  case BLFM_IR_CMD_STAR:
    // All servos to minimum (-90°)
    out->servo1.proportional_input = -1000;
    out->servo2.proportional_input = -1000;
    out->servo3.proportional_input = -1000;
    out->servo4.proportional_input = -1000;
    break;
  case BLFM_IR_CMD_0:
    // All servos to center (0°)
    out->servo1.proportional_input = 0;
    out->servo2.proportional_input = 0;
    out->servo3.proportional_input = 0;
    out->servo4.proportional_input = 0;
    break;
  case BLFM_IR_CMD_HASH:
    // All servos to maximum (+90°)
    out->servo1.proportional_input = 1000;
    out->servo2.proportional_input = 1000;
    out->servo3.proportional_input = 1000;
    out->servo4.proportional_input = 1000;
    break;

  default:
    break;
  }

  if (blfm_system_state.current_mode != BLFM_MODE_MANUAL)
    return;

  switch (in->command) {
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

  case BLFM_IR_CMD_OK:
  default:
    speed = 0;
    blfm_system_state.motion_state = BLFM_MOTION_STOP;
    break;
  }

  int angle = motion_state_to_angle(blfm_system_state.motion_state);
  set_motor_motion_by_angle(angle, speed, &out->motor);
}
#endif /* BLFM_ENABLED_IR_REMOTE */


#if BLFM_ENABLED_MODE_BUTTON
void blfm_controller_process_mode_button(const blfm_mode_button_event_t *event,
                                         blfm_actuator_command_t *command) {
  static uint32_t last_press_tick = 0;
  (void)command;

  if (event && event->event_type == BLFM_MODE_BUTTON_EVENT_PRESSED) {
    uint32_t now = xTaskGetTickCount();
    if ((now - last_press_tick) > pdMS_TO_TICKS(MODE_BUTTON_DEBOUNCE_MS)) {
      blfm_gpio_toggle_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
      last_press_tick = now;
    }
  }
}
#endif /* BLFM_ENABLED_MODE_BUTTON */

#if BLFM_ENABLED_IR_REMOTE
bool blfm_controller_check_ir_timeout(blfm_actuator_command_t *out) {
  (void)out;
  return false;
}
#endif /* BLFM_ENABLED_IR_REMOTE */


#if BLFM_ENABLED_ESP32
void blfm_controller_process_esp32(const blfm_esp32_event_t *event,
                                   blfm_actuator_command_t *out) {
  memset(out, 0, sizeof(*out));
  if (!event || !out)
    return;

  int angle = 0;
  int speed = event->speed;

  switch (event->command) {
  case BLFM_ESP32_CMD_UP:
    angle = 0;
    break;
  case BLFM_ESP32_CMD_DOWN:
    angle = 180;
    break;
  case BLFM_ESP32_CMD_LEFT:
    angle = -90;
    break;
  case BLFM_ESP32_CMD_RIGHT:
    angle = 90;
    break;
  default:
    angle = 0;
    speed = 0;
    break;
  }

  set_motor_motion_by_angle(angle, speed, &out->motor);
}
#endif /* BLFM_ENABLED_ESP32 */
