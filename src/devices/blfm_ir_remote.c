
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_ir_remote.h"
#include "blfm_interrupts.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdbool.h>

/* Timing Definitions (microseconds) */
#define NEC_BITS 32
#define NEC_HDR_MARK_MIN 8500
#define NEC_HDR_MARK_MAX 9500
#define NEC_HDR_SPACE_MIN 4000
#define NEC_HDR_SPACE_MAX 5000
#define NEC_BIT_MARK_MIN 400
#define NEC_BIT_MARK_MAX 700
#define NEC_ZERO_SPACE_MIN 300
#define NEC_ZERO_SPACE_MAX 800
#define NEC_ONE_SPACE_MIN 1300
#define NEC_ONE_SPACE_MAX 2200
#define NEC_REPEAT_SPACE_MIN 2000
#define NEC_REPEAT_SPACE_MAX 2500

#define NEC_REPEAT_TIMEOUT_MS 250

/* State Definitions */
typedef enum {
  NEC_STATE_IDLE,
  NEC_STATE_LEAD_SPACE,
  NEC_STATE_BIT_MARK,
  NEC_STATE_BIT_SPACE
} nec_state_t;

static nec_state_t nec_state = NEC_STATE_IDLE;
static uint32_t nec_data = 0;
static uint32_t bit_index = 0;
static uint32_t last_edge_time = 0;
static QueueHandle_t ir_event_queue = NULL;
static uint32_t last_valid_command_tick = 0;

/* Queue Event from ISR */
static void queue_event(blfm_ir_command_t command, uint32_t pulse_us) {
  if (!ir_event_queue) return;
  
  blfm_ir_remote_event_t event = {
    .command = command,
    .pulse_us = pulse_us,
    .timestamp = xTaskGetTickCountFromISR()
  };
  
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(ir_event_queue, &event, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* NEC Pulse Decoder */
static blfm_ir_command_t ir_remote_process_pulse(uint32_t pulse_us,
                                                 bool is_low) {
  static blfm_ir_command_t last_cmd = BLFM_IR_CMD_NONE;

  if (is_low) {
    // MARK
    switch (nec_state) {
    case NEC_STATE_IDLE:
      if (pulse_us > NEC_HDR_MARK_MIN && pulse_us < NEC_HDR_MARK_MAX) {
        nec_state = NEC_STATE_LEAD_SPACE;
      }
      break;

    case NEC_STATE_BIT_MARK:
      if (pulse_us > NEC_BIT_MARK_MIN && pulse_us < NEC_BIT_MARK_MAX) {
        nec_state = NEC_STATE_BIT_SPACE;
      } else {
        nec_state = NEC_STATE_IDLE;
      }
      break;

    default:
      break;
    }
  } else {
    // SPACE
    switch (nec_state) {
    case NEC_STATE_LEAD_SPACE:
      if (pulse_us > NEC_HDR_SPACE_MIN && pulse_us < NEC_HDR_SPACE_MAX) {
        nec_data = 0;
        bit_index = 0;
        nec_state = NEC_STATE_BIT_MARK;

      } else if (pulse_us > NEC_REPEAT_SPACE_MIN &&
                 pulse_us < NEC_REPEAT_SPACE_MAX) {
        uint32_t now = xTaskGetTickCount();
        // Accept REPEAT only if received within 150ms of the LAST VALID command
        if ((now - last_valid_command_tick) < pdMS_TO_TICKS(150)) {
          // Still valid repeat
          nec_state = NEC_STATE_IDLE;
          return last_cmd;
        } else {
          // Too old — treat as NONE
          nec_state = NEC_STATE_IDLE;
          return BLFM_IR_CMD_NONE;
        }
      }

      else {
        nec_state = NEC_STATE_IDLE;
      }
      break;

    case NEC_STATE_BIT_SPACE:
      if (pulse_us > NEC_ONE_SPACE_MIN && pulse_us < NEC_ONE_SPACE_MAX) {
        nec_data |= (1UL << bit_index);
      } else if (pulse_us > NEC_ZERO_SPACE_MIN &&
                 pulse_us < NEC_ZERO_SPACE_MAX) {
        nec_data &= ~(1UL << bit_index);
      } else {
        nec_state = NEC_STATE_IDLE;
        break;
      }

      bit_index++;
      if (bit_index >= NEC_BITS) {
        uint8_t command = (nec_data >> 16) & 0xFF;
        uint8_t command_inv = (nec_data >> 24) & 0xFF;

        if ((command ^ command_inv) == 0xFF) {
          last_cmd = (blfm_ir_command_t)command;
          last_valid_command_tick = xTaskGetTickCount();
          nec_state = NEC_STATE_IDLE;
          return last_cmd;
        } else {
          last_cmd = BLFM_IR_CMD_NONE;
          nec_state = NEC_STATE_IDLE;
          return BLFM_IR_CMD_NONE;
        }
      } else {
        nec_state = NEC_STATE_BIT_MARK;
      }
      break;

    default:
      break;
    }
  }

  return BLFM_IR_CMD_NONE;
}

/* Interrupt Handler */
static void ir_exti_handler(void) {
  if (!ir_event_queue) {
    return;
  }

  uint32_t now = DWT->CYCCNT;

  if (last_edge_time == 0) {
    last_edge_time = now;
    return;
  }

  uint32_t diff = now - last_edge_time;
  uint32_t pulse_us = diff / 72;

  bool is_low = ((GPIOA->IDR & (1U << BLFM_IR_REMOTE_PIN)) == 0);
  bool is_mark = !is_low;

  blfm_ir_command_t cmd = ir_remote_process_pulse(pulse_us, is_mark);
  if (cmd != BLFM_IR_CMD_NONE) {
    queue_event(cmd, pulse_us);
  }

  last_edge_time = now;
}

/* Initialization */
void blfm_ir_remote_init(QueueHandle_t event_queue) {
  ir_event_queue = event_queue;

  // Enable DWT for precise timings
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  // Configure GPIO
  blfm_gpio_config_input_pullup((uint32_t)BLFM_IR_REMOTE_PORT,
                                BLFM_IR_REMOTE_PIN);

  /* GPIO port A = 0, B = 1, C = 2, etc. */
  uint8_t gpio_port = 0; /* Port A */
  blfm_exti_init(gpio_port, BLFM_IR_REMOTE_PIN, BLFM_EXTI_TRIGGER_BOTH, ir_exti_handler);
}
