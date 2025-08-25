
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
#include "s17_protocol.h"
#include "S17_config.h"
#include "s17_security.h"
#include "s17_nrf24l01.h"

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

  blfm_led_apply(&cmd->led);
  blfm_motor_apply(&cmd->motor);

  // Apply commands to all 4 servos
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
    
    /* Prepare S17 test broadcast */
    uint8_t test_data[S17_MAX_PLAINTEXT_SIZE];
    
    /* Use standardized S17 packet format */
    s17_pack_header(test_data, S17_THIS_DEVICE_ID, S17_BROADCAST);
    
    /* Test message payload */
    test_data[S17_PAYLOAD_OFFSET + 0] = 0x77;  /* Test message identifier */
    test_data[S17_PAYLOAD_OFFSET + 1] = 0x01;  /* Message type: periodic */
    test_data[S17_PAYLOAD_OFFSET + 2] = (test_message_counter >> 8) & 0xFF;  /* Counter high */
    test_data[S17_PAYLOAD_OFFSET + 3] = test_message_counter & 0xFF;         /* Counter low */
    
    /* Clear remaining payload */
    for (int i = S17_PAYLOAD_OFFSET + 4; i < S17_MAX_PLAINTEXT_SIZE; i++) {
      test_data[i] = 0x00;
    }
    
    /* Send raw NRF24 data (no S17 encryption) for compatibility with nrf24+ receiver */
    uint8_t raw_data[32];
    
    /* Create nrf24+ compatible message format */
    raw_data[0] = 'P';
    raw_data[1] = 'k';
    raw_data[2] = 't';
    raw_data[3] = ' ';
    raw_data[4] = '0' + ((test_message_counter % 3) + 1);  /* Packet 1, 2, or 3 */
    raw_data[5] = '-';
    raw_data[6] = '0' + (test_message_counter / 100) % 10;
    raw_data[7] = '0' + (test_message_counter / 10) % 10;
    raw_data[8] = '0' + test_message_counter % 10;
    raw_data[9] = '\0';
    
    /* Fill rest with spaces/nulls */
    for (int i = 10; i < 32; i++) {
      raw_data[i] = 0x00;
    }
    
    /* Send directly via NRF24 (bypass S17 encryption) */
    uint8_t target_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE8};  /* nrf24+ receiver address */
    s17_nrf24_transmit_to(target_addr, raw_data, 32);
    
    /* Blink onboard LED 3 times fast when sending broadcast */
    for (int i = 0; i < 3; i++) {
      blfm_gpio_clear_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);  /* ON (active LOW) */
      for (volatile uint32_t j = 0; j < 500000; j++) __NOP();  /* ~25ms on */
      blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);    /* OFF (active LOW) */
      for (volatile uint32_t j = 0; j < 500000; j++) __NOP();  /* ~25ms off */
    }
  }
}
