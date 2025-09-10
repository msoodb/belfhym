
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_button.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_interrupts.h"
#include "blfm_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(50)

static QueueHandle_t button_event_queue = NULL;
static uint32_t last_interrupt_tick = 0;

static void queue_event(blfm_button_event_type_t type) {
  if (!button_event_queue) return;
  
  blfm_button_event_t event = {
    .event_type = type,
    .timestamp = xTaskGetTickCountFromISR()
  };
  
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(button_event_queue, &event, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void button_exti_handler(void) {
  if (!button_event_queue) return;
  
  uint32_t now = xTaskGetTickCountFromISR();
  if ((now - last_interrupt_tick) < BUTTON_DEBOUNCE_TICKS) {
    return;
  }
  last_interrupt_tick = now;
  
  bool pressed = (blfm_gpio_read_pin((uint32_t)BLFM_BUTTON_PORT, BLFM_BUTTON_PIN) == 0);
  
  if (pressed) {
    queue_event(BLFM_BUTTON_EVENT_PRESSED);
  } else {
    queue_event(BLFM_BUTTON_EVENT_RELEASED);
  }
}

void blfm_button_init(QueueHandle_t event_queue) {
  button_event_queue = event_queue;
  blfm_gpio_config_input_pullup((uint32_t)BLFM_BUTTON_PORT, BLFM_BUTTON_PIN);
  
  uint8_t gpio_port = 0;
  blfm_exti_init(gpio_port, BLFM_BUTTON_PIN, BLFM_EXTI_TRIGGER_BOTH, button_exti_handler);
}

