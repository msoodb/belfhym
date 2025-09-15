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
#include "S17.h"
#include "S17_config.h"
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
  
  /* LED disabled - no blinking */
  out->led.mode = BLFM_LED_MODE_OFF;

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


static void blfm_controller_process_nrf24(const blfm_nrf24_event_t *event,
                                           blfm_controller_output_t *command) {
  if (!event || !command) return;
  
  /* S17 callback has already filtered and parsed the message */
  /* event->sender_id contains the sender, event->data contains clean payload */
  
  if (event->length >= 8 && event->data[0] == 0x4A && event->data[1] == 0x01) {
    
    blfm_joystick_event_t joystick_event;
    
    joystick_event.x_normalized = (int16_t)(
      (uint16_t)event->data[2] | 
      ((uint16_t)event->data[3] << 8)
    );
    
    joystick_event.y_normalized = (int16_t)(
      (uint16_t)event->data[4] | 
      ((uint16_t)event->data[5] << 8)
    );
    
    joystick_event.button_pressed = (event->data[6] == 0x01);
    joystick_event.timestamp = xTaskGetTickCount();
    
    blfm_controller_process_joystick(&joystick_event, command);

    return;
  }

  if (event->length >= 12 && event->data[0] == 0x4A && event->data[1] == 0x02) {
    
    blfm_joystick_event_t joystick_event;
    
    joystick_event.x_normalized = (int16_t)(
      (uint16_t)event->data[2] | 
      ((uint16_t)event->data[3] << 8)
    );
    
    joystick_event.y_normalized = (int16_t)(
      (uint16_t)event->data[4] | 
      ((uint16_t)event->data[5] << 8)
    );
    
    joystick_event.button_pressed = (event->data[10] & 0x01) != 0;
    joystick_event.timestamp = xTaskGetTickCount();
    
    blfm_controller_process_joystick(&joystick_event, command);

    return;  /* Exit early, joystick data processed */
  }
  
  /* Check application payload for test messages */
  if (event->length >= 1 && event->data[0] == 0x77) {
    /* Test broadcast packet received - no visual indication */
  }
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
    
    /* LED disabled */
    command->led.mode = BLFM_LED_MODE_OFF;
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
  /* LED disabled */
  command->led.mode = BLFM_LED_MODE_OFF;
}
