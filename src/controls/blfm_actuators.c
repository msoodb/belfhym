
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_led.h"
#include "blfm_motor.h"
#include "blfm_servomotor.h"
#include "blfm_actuators.h"
#include "blfm_types.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "S17.h"
#include "S17_config.h"
#include "FreeRTOS.h"
#include "task.h"

void blfm_actuator_hub_init(void) {
  blfm_led_init();
  blfm_motor_init();
  blfm_servomotor_init();
  
  // Set servo types and enable all 4 servos
  blfm_servomotor_set_type(0, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo1
  blfm_servomotor_set_type(1, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo2
  blfm_servomotor_set_type(2, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo3
  blfm_servomotor_set_type(3, BLFM_SERVO_TYPE_PROPORTIONAL);  // Servo4
  
  // Enable all servos
  blfm_servomotor_enable(0);
  blfm_servomotor_enable(1);
  blfm_servomotor_enable(2);
  blfm_servomotor_enable(3);
}

void blfm_actuator_hub_apply(const blfm_controller_output_t *cmd) {
  if (!cmd)
    return;

  blfm_servomotor_update_time(xTaskGetTickCount());

  blfm_led_apply(&cmd->led);
  blfm_motor_apply(&cmd->motor);

  blfm_servomotor_apply(0, &cmd->servo1);
  blfm_servomotor_apply(1, &cmd->servo2);
  blfm_servomotor_apply(2, &cmd->servo3);
  blfm_servomotor_apply(3, &cmd->servo4);
  
  // Handle S17 broadcast command if requested by controller
  if (cmd->nrf24.should_broadcast && cmd->nrf24.length > 0) {
    s17_broadcast(cmd->nrf24.dest_id, cmd->nrf24.data, cmd->nrf24.length);
  }
}

void blfm_actuator_hub_apply_periodic(uint32_t current_time_ms) {

  return;
  
  static uint32_t last_broadcast_time = 0;
  static uint16_t test_message_counter = 0;
  
  /* Periodic test broadcast every 3 seconds */
  if ((current_time_ms - last_broadcast_time) >= 3000) {
    last_broadcast_time = current_time_ms;
    test_message_counter++;
    
    /* Create test broadcast payload - S17 handles packet formatting internally */
    uint8_t test_payload[8] = {
      0x77,  /* Test message identifier */
      0x01,  /* Message type: periodic */
      (test_message_counter >> 8) & 0xFF,  /* Counter high */
      test_message_counter & 0xFF,         /* Counter low */
      0x00, 0x00, 0x00, 0x00  /* Reserved */
    };
    
    /* Send using modern S17 API - handles all packet formatting, encryption, etc. */
    s17_result_t result = s17_broadcast(0, test_payload, sizeof(test_payload));
    
    /* Check send result */
    (void)result;  /* For now, just continue even if send fails */
  }
}
