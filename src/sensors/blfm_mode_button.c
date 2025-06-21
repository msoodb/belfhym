

/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_mode_button.h"
#include "FreeRTOS.h"
#include "blfm_exti_dispatcher.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "queue.h"
#include "stm32f1xx.h"

#define DEBOUNCE_DELAY_MS 50

static QueueHandle_t mode_button_queue = NULL;
static uint32_t last_press_tick = 0;

static void mode_button_exti_handler(void) {
  if (mode_button_queue == NULL) {
    return;
  }

  uint32_t now = xTaskGetTickCountFromISR();
  if ((now - last_press_tick) < pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)) {
    return; // Too soon, ignore as bounce
  }
  last_press_tick = now;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  uint32_t pin_state = blfm_gpio_read_pin((uint32_t)BLFM_MODE_BUTTON_PORT, BLFM_MODE_BUTTON_PIN);
  blfm_mode_button_event_t event;

  if (pin_state) {
    event.event_type = BLFM_MODE_BUTTON_EVENT_RELEASED;
  } else {
    event.event_type = BLFM_MODE_BUTTON_EVENT_PRESSED;
  }

  event.timestamp = now;

  xQueueSendFromISR(mode_button_queue, &event, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void blfm_mode_button_init(QueueHandle_t controller_queue) {
  mode_button_queue = controller_queue;

  // Configure as input with Pull‑Up
  blfm_gpio_config_input_pullup((uint32_t)BLFM_MODE_BUTTON_PORT, BLFM_MODE_BUTTON_PIN);

  // EXTI mapping for PA4
  AFIO->EXTICR[1] &= ~(0xF << (4 * (BLFM_MODE_BUTTON_PIN - 4)));
  AFIO->EXTICR[1] |= (AFIO_EXTICR2_EXTI4_PA << (4 * (BLFM_MODE_BUTTON_PIN - 4)));

  // Trigger both edges
  EXTI->IMR |= (1 << BLFM_MODE_BUTTON_PIN);
  EXTI->RTSR |= (1 << BLFM_MODE_BUTTON_PIN);
  EXTI->FTSR |= (1 << BLFM_MODE_BUTTON_PIN);

  // Register callback
  blfm_exti_register_callback(BLFM_MODE_BUTTON_PIN, mode_button_exti_handler);

  // Enable IRQ
  NVIC_EnableIRQ(EXTI4_IRQn);
}
