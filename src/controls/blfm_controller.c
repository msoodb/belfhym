/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_controller.h"
#include "FreeRTOS.h"
#include "blfm_state.h"
#include "blfm_types.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "S17_config.h"  /* Use S17 config instead */
#include "s17_protocol.h"  /* S17 standardized packet format */
#include "s17_security.h"  /* For S17_MAX_PLAINTEXT_SIZE */
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define ULTRASONIC_FORWARD_THRESH 20
#define MOTOR_BACKWARD_TICKS_MAX 2
#define MOTOR_MIN_ROTATE_TICKS 4
#define MOTOR_MAX_ROTATE_TICKS 6
#define MOTOR_DEFAULT_SPEED 255

/* Static function declarations - internal use only */
static void blfm_controller_process(const blfm_sensor_data_t *in,
                                    blfm_controller_output_t *out);
static void blfm_controller_process_button(const blfm_button_event_t *event,
                                           blfm_controller_output_t *command);
static void blfm_controller_process_nrf24(const blfm_nrf24_event_t *event,
                                          blfm_controller_output_t *command);
static void blfm_controller_process_joystick(const blfm_joystick_event_t *event,
                                             blfm_controller_output_t *command);

static int motor_backward_ticks = 0;
static int motor_rotate_ticks = 0;
static int motor_rotate_duration = 0;

static blfm_system_state_t blfm_system_state;

void blfm_controller_init(void) {
  blfm_system_state.current_mode = BLFM_MODE_MANUAL;
  blfm_system_state.motion_state = BLFM_MOTION_STOP;
  motor_backward_ticks = 0;
  motor_rotate_ticks = 0;
  motor_rotate_duration = 0;
}

/* Unified controller input processing - single entry point for ALL inputs */
void blfm_controller_process_input(const blfm_controller_input_t *input,
                                   blfm_controller_output_t *command) {
  if (!input || !command) return;
  
  /* Process based on input type */
  switch (input->type) {
    case BLFM_INPUT_SENSOR:
      /* Sensor data drives continuous control (auto mode, etc.) */
      blfm_controller_process(&input->data.sensor, command);
      break;
      
    case BLFM_INPUT_BUTTON:
      /* Button events can override current state */
      blfm_controller_process_button(&input->data.button, command);
      break;
      
    case BLFM_INPUT_NRF24:
      /* NRF24 for remote commands */
      blfm_controller_process_nrf24(&input->data.nrf24, command);
      break;
      
    case BLFM_INPUT_JOYSTICK:
      /* Joystick for motor control from hermes */
      blfm_controller_process_joystick(&input->data.joystick, command);
      break;
      
    default:
      /* Initialize safe defaults */
      command->motor.left.speed = 0;
      command->motor.left.direction = 0;
      command->motor.right.speed = 0;
      command->motor.right.direction = 0;
      command->servo1.proportional_input = 0x7FFF;
      command->servo2.proportional_input = 0x7FFF;
      command->servo3.proportional_input = 0x7FFF;
      command->servo4.proportional_input = 0x7FFF;
      command->nrf24.should_broadcast = false;
      command->nrf24.length = 0;
      break;
  }
}

static void blfm_controller_process(const blfm_sensor_data_t *in,
                                     blfm_controller_output_t *out) {
  if (!in || !out)
    return;

  /* Clear motor commands */
  out->motor.left.speed = 0;
  out->motor.left.direction = 0;
  out->motor.right.speed = 0;
  out->motor.right.direction = 0;
  
  /* Initialize servo commands - no change by default */
  out->servo1.proportional_input = 0x7FFF;  /* Special value: no change */
  out->servo2.proportional_input = 0x7FFF;  /* Special value: no change */
  out->servo3.proportional_input = 0x7FFF;  /* Special value: no change */
  out->servo4.proportional_input = 0x7FFF;  /* Special value: no change */
  
  /* Initialize NRF24 command - no broadcast by default */
  out->nrf24.should_broadcast = false;
  out->nrf24.length = 0;
  
  /* Set LED based on current mode */
  switch (blfm_system_state.current_mode) {
    case BLFM_MODE_MANUAL:
      out->led.mode = BLFM_LED_MODE_BLINK;
      out->led.on_time = 100;   /* Manual: quick flash - 100ms on */
      out->led.off_time = 900;  /* Manual: quick flash - 900ms off */
      break;
    case BLFM_MODE_AUTO:
      out->led.mode = BLFM_LED_MODE_BLINK;
      out->led.on_time = 250;   /* Auto: medium blink - 250ms on */
      out->led.off_time = 250;  /* Auto: medium blink - 250ms off */
      break;
    case BLFM_MODE_EMERGENCY:
      out->led.mode = BLFM_LED_MODE_ON; /* Emergency: solid on */
      /* Emergency mode: Stop all servos at center position for safety */
      out->servo1.proportional_input = 0;  /* Center position */
      out->servo2.proportional_input = 0;  /* Center position */
      out->servo3.proportional_input = 0;  /* Center position */
      out->servo4.proportional_input = 0;  /* Center position */
      break;
  }

  if (blfm_system_state.current_mode == BLFM_MODE_AUTO) {
    /* Auto mode: Simple obstacle avoidance */
    if (in->ultrasonic.distance_mm <= ULTRASONIC_FORWARD_THRESH) {
      /* Obstacle detected - back up then turn */
      if (motor_backward_ticks < MOTOR_BACKWARD_TICKS_MAX) {
        /* Backing up */
        out->motor.left.direction = 1;   /* Backward */
        out->motor.right.direction = 1;  /* Backward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        motor_backward_ticks++;
      } else {
        /* Turn left */
        out->motor.left.direction = 1;   /* Left motor backward */
        out->motor.right.direction = 0;  /* Right motor forward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        motor_rotate_ticks++;
        
        if (motor_rotate_ticks >= MOTOR_MIN_ROTATE_TICKS) {
          /* Reset counters */
          motor_backward_ticks = 0;
          motor_rotate_ticks = 0;
        }
      }
    } else {
      /* No obstacle - go forward */
      out->motor.left.direction = 0;   /* Forward */
      out->motor.right.direction = 0;  /* Forward */
      out->motor.left.speed = MOTOR_DEFAULT_SPEED;
      out->motor.right.speed = MOTOR_DEFAULT_SPEED;
      motor_backward_ticks = 0;
      motor_rotate_ticks = 0;
    }
  }

  if (blfm_system_state.current_mode == BLFM_MODE_MANUAL) {
    /* Manual mode: Apply current motion state directly */
    switch (blfm_system_state.motion_state) {
      case BLFM_MOTION_FORWARD:
        out->motor.left.direction = 0;   /* Forward */
        out->motor.right.direction = 0;  /* Forward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        break;
        
      case BLFM_MOTION_BACKWARD:
        out->motor.left.direction = 1;   /* Backward */
        out->motor.right.direction = 1;  /* Backward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        break;
        
      case BLFM_MOTION_ROTATE_LEFT:
        out->motor.left.direction = 1;   /* Left motor backward */
        out->motor.right.direction = 0;  /* Right motor forward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        break;
        
      case BLFM_MOTION_ROTATE_RIGHT:
        out->motor.left.direction = 0;   /* Left motor forward */
        out->motor.right.direction = 1;  /* Right motor backward */
        out->motor.left.speed = MOTOR_DEFAULT_SPEED;
        out->motor.right.speed = MOTOR_DEFAULT_SPEED;
        break;
        
      case BLFM_MOTION_STOP:
      default:
        /* Motors already cleared above */
        break;
    }
  }
}

static void blfm_controller_process_button(const blfm_button_event_t *event,
                                            blfm_controller_output_t *command) {
  if (!event || !command) return;
  
  /* Simple button press = toggle DEBUG LED */
  if (event->event_type == BLFM_BUTTON_EVENT_PRESSED) {
    /* Toggle DEBUG LED */
    static bool debug_led_state = false;
    debug_led_state = !debug_led_state;
    
    if (debug_led_state) {
      blfm_gpio_set_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
    } else {
      blfm_gpio_clear_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
    }
  }
}

static void blfm_controller_process_nrf24(const blfm_nrf24_event_t *event,
                                           blfm_controller_output_t *command) {
  if (!event || !command) return;
  
  /* Verify packet has minimum size for S17 header */
  if (event->length < S17_HEADER_SIZE) {
    return; /* Invalid packet - too small */
  }
  
  /* Use standardized S17 header for self-filtering */
  if (s17_packet_is_from_us(event->data, S17_THIS_DEVICE_ID)) {
    return; /* Ignore our own broadcast */
  }
  
  /* Check if packet is addressed to us or broadcast */
  if (!s17_packet_is_for_us(event->data, S17_THIS_DEVICE_ID)) {
    return; /* Not for us */
  }
  
  /* Extract S17 header for logging/debugging */
  s17_header_t header = s17_unpack_header(event->data);
  
  /* Check application payload for joystick messages */
  if (event->length >= S17_PAYLOAD_OFFSET + 8 && 
      event->data[S17_PAYLOAD_OFFSET] == 0x4A) {  /* 'J' for Joystick */
    
    /* Decode joystick packet from hermes */
    blfm_joystick_event_t joystick_event;
    
    /* Extract X coordinate (little-endian) */
    joystick_event.x_normalized = (int16_t)(
      (uint16_t)event->data[S17_PAYLOAD_OFFSET + 2] | 
      ((uint16_t)event->data[S17_PAYLOAD_OFFSET + 3] << 8)
    );
    
    /* Extract Y coordinate (little-endian) */
    joystick_event.y_normalized = (int16_t)(
      (uint16_t)event->data[S17_PAYLOAD_OFFSET + 4] | 
      ((uint16_t)event->data[S17_PAYLOAD_OFFSET + 5] << 8)
    );
    
    /* Extract button state */
    joystick_event.button_pressed = (event->data[S17_PAYLOAD_OFFSET + 6] == 0x01);
    joystick_event.timestamp = xTaskGetTickCount();
    
    /* Process decoded joystick data */
    blfm_controller_process_joystick(&joystick_event, command);
    
    /* Visual feedback: blink onboard LED when joystick data received */
    blfm_gpio_clear_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);  /* ON (active LOW) */
    for (uint32_t j = 0; j < 100000; j++) __NOP();  /* ~50ms on */
    blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);    /* OFF (active LOW) */
    
    return;  /* Exit early, joystick data processed */
  }
  
  /* Check application payload for test messages */
  if (event->length >= S17_PAYLOAD_OFFSET + 1 && 
      event->data[S17_PAYLOAD_OFFSET] == 0x77) {
    
    /* Test broadcast packet received - single slow blink */
    blfm_gpio_clear_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);  /* ON (active LOW) */
    for (uint32_t j = 0; j < 200000; j++) __NOP();  /* ~100ms on */
    blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);    /* OFF (active LOW) */
  }
  
  /* Suppress unused variable warning */
  (void)header;
}

static void blfm_controller_process_joystick(const blfm_joystick_event_t *event,
                                             blfm_controller_output_t *command) {
  if (!event || !command) return;
  
  /* Initialize all command fields to safe defaults (following belfhym pattern) */
  command->servo1.proportional_input = 0x7FFF;  /* Servo neutral position */
  command->servo2.proportional_input = 0x7FFF;
  command->servo3.proportional_input = 0x7FFF;
  command->servo4.proportional_input = 0x7FFF;
  command->nrf24.should_broadcast = false;
  command->nrf24.length = 0;
  
  /* Only allow joystick motor control in MANUAL mode (following belfhym pattern) */
  if (blfm_system_state.current_mode != BLFM_MODE_MANUAL) {
    /* Not in manual mode - don't control motors */
    command->motor.left.speed = 0;
    command->motor.left.direction = 0;
    command->motor.right.speed = 0;
    command->motor.right.direction = 0;
    
    /* Set LED to indicate joystick input received but mode not manual */
    command->led.mode = BLFM_LED_MODE_BLINK;
    command->led.on_time = 300;   /* Long slow blink = wrong mode */
    command->led.off_time = 300;
    return;
  }
  
  /* Simple joystick control to avoid oscillation */
  /* Joystick coordinates: X = -1000 (left) to +1000 (right) */
  /*                      Y = -1000 (backward) to +1000 (forward) */
  
  int16_t x = event->x_normalized;  /* Steering */
  int16_t y = event->y_normalized;  /* Throttle */
  
  /* Deadband to prevent oscillation from small joystick movements */
  if (y > -100 && y < 100 && x > -100 && x < 100) {
    /* Joystick in center deadband - stop motors */
    command->motor.left.speed = 0;
    command->motor.left.direction = 0;
    command->motor.right.speed = 0;
    command->motor.right.direction = 0;
    return;
  }
  
  /* Simple control: Y-axis controls both motors for forward/backward */
  /* X-axis modifies left/right motors for steering */
  
  if (y > 100) {
    /* Forward movement */
    command->motor.left.direction = 0;   /* Forward */
    command->motor.right.direction = 0;  /* Forward */
    command->motor.left.speed = 255;
    command->motor.right.speed = 255;
    
    /* Apply steering - reduce speed on one side */
    if (x > 100) {
      /* Turn right - slow down right motor */
      command->motor.right.speed = 100;
    } else if (x < -100) {
      /* Turn left - slow down left motor */
      command->motor.left.speed = 100;
    }
    
  } else if (y < -100) {
    /* Backward movement */
    command->motor.left.direction = 1;   /* Backward */
    command->motor.right.direction = 1;  /* Backward */
    command->motor.left.speed = 255;
    command->motor.right.speed = 255;
    
    /* Apply steering - reduce speed on one side */
    if (x > 100) {
      /* Turn right - slow down right motor */
      command->motor.right.speed = 100;
    } else if (x < -100) {
      /* Turn left - slow down left motor */
      command->motor.left.speed = 100;
    }
    
  } else {
    /* Only steering (Y near zero) */
    if (x > 100) {
      /* Rotate right */
      command->motor.left.direction = 0;   /* Forward */
      command->motor.right.direction = 1;  /* Backward */
      command->motor.left.speed = 255;
      command->motor.right.speed = 255;
    } else if (x < -100) {
      /* Rotate left */
      command->motor.left.direction = 1;   /* Backward */
      command->motor.right.direction = 0;  /* Forward */
      command->motor.left.speed = 255;
      command->motor.right.speed = 255;
    } else {
      /* Stop */
      command->motor.left.speed = 0;
      command->motor.right.speed = 0;
    }
  }
  
  /* No need for minimum speed threshold - already using 255 for radio commands */
  
  /* Button press can be used for emergency stop */
  if (event->button_pressed) {
    /* Emergency stop when button pressed */
    command->motor.left.speed = 0;
    command->motor.right.speed = 0;
  }
  
  /* Debug: Visual indication of motor commands via LED timing */
  if (command->motor.left.speed > 0 || command->motor.right.speed > 0) {
    /* Motor commands are being sent - faster blink */
    command->led.mode = BLFM_LED_MODE_BLINK;
    command->led.on_time = 50;   
    command->led.off_time = 50;
  } else {
    /* No motor commands - slower blink */
    command->led.mode = BLFM_LED_MODE_BLINK;
    command->led.on_time = 200;   
    command->led.off_time = 200;
  }
}
