/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_ultrasonic.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include <stdbool.h>

/* Timing constants */
#define TRIGGER_PULSE_US 10
#define ECHO_TIMEOUT_MS 30
#define SOUND_SPEED_DIV 58  /* Distance in cm = time in us / 58 */

/* DWT cycle counter for microsecond timing */
static void delay_us(uint32_t us) {
  uint32_t start = DWT->CYCCNT;
  uint32_t cycles = us * (SystemCoreClock / 1000000);
  while ((DWT->CYCCNT - start) < cycles);
}

/* Wait for pin state with timeout */
static bool wait_for_pin(uint32_t port, uint32_t pin, int target_state, uint32_t timeout_ms) {
  uint32_t start = DWT->CYCCNT;
  uint32_t timeout_cycles = (SystemCoreClock / 1000) * timeout_ms;
  
  while (blfm_gpio_read_pin(port, pin) != target_state) {
    if ((DWT->CYCCNT - start) > timeout_cycles) {
      return false;
    }
  }
  return true;
}

/* Initialize ultrasonic sensor */
void blfm_ultrasonic_init(void) {
  /* Configure GPIO pins */
  blfm_gpio_config_output((uint32_t)BLFM_ULTRASONIC_TRIG_PORT, BLFM_ULTRASONIC_TRIG_PIN);
  blfm_gpio_config_input_pullup((uint32_t)BLFM_ULTRASONIC_ECHO_PORT, BLFM_ULTRASONIC_ECHO_PIN);
  blfm_gpio_clear_pin((uint32_t)BLFM_ULTRASONIC_TRIG_PORT, BLFM_ULTRASONIC_TRIG_PIN);
  
  /* Enable DWT cycle counter if not already enabled */
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  }
  if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}

/* Read distance from ultrasonic sensor (blocking) */
bool blfm_ultrasonic_read(blfm_ultrasonic_data_t *data) {
  if (!data) return false;
  
  /* Send trigger pulse */
  blfm_gpio_clear_pin((uint32_t)BLFM_ULTRASONIC_TRIG_PORT, BLFM_ULTRASONIC_TRIG_PIN);
  delay_us(2);
  blfm_gpio_set_pin((uint32_t)BLFM_ULTRASONIC_TRIG_PORT, BLFM_ULTRASONIC_TRIG_PIN);
  delay_us(TRIGGER_PULSE_US);
  blfm_gpio_clear_pin((uint32_t)BLFM_ULTRASONIC_TRIG_PORT, BLFM_ULTRASONIC_TRIG_PIN);
  
  /* Wait for echo to go high */
  if (!wait_for_pin((uint32_t)BLFM_ULTRASONIC_ECHO_PORT, 
                    BLFM_ULTRASONIC_ECHO_PIN, 1, ECHO_TIMEOUT_MS)) {
    data->distance_mm = 0xFFFF;  /* Indicate no echo received */
    return false;
  }
  
  /* Measure echo pulse width */
  uint32_t start = DWT->CYCCNT;
  if (!wait_for_pin((uint32_t)BLFM_ULTRASONIC_ECHO_PORT, 
                    BLFM_ULTRASONIC_ECHO_PIN, 0, ECHO_TIMEOUT_MS)) {
    data->distance_mm = 0xFFFF;  /* Indicate timeout */
    return false;
  }
  uint32_t end = DWT->CYCCNT;
  
  /* Calculate distance */
  uint32_t duration = end - start;
  uint32_t us = duration / (SystemCoreClock / 1000000);
  
  /* Convert to millimeters (us / 58 gives cm, * 10 gives mm) */
  uint32_t distance_mm = (us * 10) / SOUND_SPEED_DIV;
  
  /* Limit to sensor range (2cm to 4m) */
  if (distance_mm < 20) {
    distance_mm = 20;
  } else if (distance_mm > 4000) {
    distance_mm = 4000;
  }
  
  data->distance_mm = (uint16_t)distance_mm;
  return true;
}