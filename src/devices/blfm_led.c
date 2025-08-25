
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */


#include "blfm_led.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_types.h"
#include "FreeRTOS.h"
#include "task.h"


void blfm_led_init(void) {
  blfm_gpio_config_output((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);
  blfm_gpio_config_output((uint32_t)BLFM_LED_EXTERNAL_PORT, BLFM_LED_EXTERNAL_PIN);
  blfm_gpio_config_output((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
  
  /* Ensure onboard LED is OFF by default (active LOW) */
  blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);
}

void blfm_led_onboard_on(void) {
  blfm_gpio_clear_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);  /* Active LOW */
}

void blfm_led_onboard_off(void) {
  blfm_gpio_set_pin((uint32_t)BLFM_LED_ONBOARD_PORT, BLFM_LED_ONBOARD_PIN);    /* Active LOW */
}

void blfm_led_external_on(void) {
  blfm_gpio_set_pin((uint32_t)BLFM_LED_EXTERNAL_PORT, BLFM_LED_EXTERNAL_PIN);
}

void blfm_led_external_off(void) {
  blfm_gpio_clear_pin((uint32_t)BLFM_LED_EXTERNAL_PORT, BLFM_LED_EXTERNAL_PIN);
}

void blfm_led_debug_on(void) {
  blfm_gpio_set_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
}

void blfm_led_debug_off(void) {
  blfm_gpio_clear_pin((uint32_t)BLFM_LED_DEBUG_PORT, BLFM_LED_DEBUG_PIN);
}

void blfm_led_apply(const blfm_led_command_t *cmd) {
  static uint32_t last_blink_time = 0;
  static bool blink_state = false;
  
  if (!cmd) return;
  
  switch (cmd->mode) {
    case BLFM_LED_MODE_OFF:
      blfm_led_external_off();
      break;
      
    case BLFM_LED_MODE_ON:
      blfm_led_external_on();
      break;
      
    case BLFM_LED_MODE_BLINK: {
      uint32_t current_time = xTaskGetTickCount();
      uint32_t interval = blink_state ? pdMS_TO_TICKS(cmd->on_time) : pdMS_TO_TICKS(cmd->off_time);
      
      if ((current_time - last_blink_time) >= interval) {
        blink_state = !blink_state;
        
        if (blink_state) {
          blfm_led_external_on();
        } else {
          blfm_led_external_off();
        }
        
        last_blink_time = current_time;
      }
      break;
    }
      
    default:
      blfm_led_external_off();
      break;
  }
}
