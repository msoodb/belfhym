/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_ir_remote.h"
#include "FreeRTOS.h"
#include "blfm_exti_dispatcher.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_types.h"
#include "queue.h"
#include "stm32f1xx.h"

#define NEC_BITS 32

#define NEC_CMD_MASK 0x0000FF00
#define NEC_CMD_SHIFT 8

#define NEC_BITS 32
#define NEC_HDR_MARK_MIN 8500 // us
#define NEC_HDR_MARK_MAX 9500
#define NEC_HDR_SPACE_MIN 4000
#define NEC_HDR_SPACE_MAX 5000
#define NEC_BIT_MARK_MIN 400
#define NEC_BIT_MARK_MAX 700
#define NEC_ZERO_SPACE_MIN 300
#define NEC_ZERO_SPACE_MAX 800
#define NEC_ONE_SPACE_MIN 1300
#define NEC_ONE_SPACE_MAX 2200

typedef enum {
  NEC_STATE_IDLE,
  NEC_STATE_LEAD_SPACE,
  NEC_STATE_BIT_MARK,
  NEC_STATE_BIT_SPACE
} nec_state_t;

static nec_state_t nec_state = NEC_STATE_IDLE;
static uint32_t nec_data = 0;
static int bit_index = 0;

static uint32_t last_edge_time = 0;
static QueueHandle_t ir_controller_queue = NULL;

static blfm_ir_command_t blfm_ir_remote_process_pulse(uint32_t pulse_us) {
  static blfm_ir_command_t last_cmd = BLFM_IR_CMD_NONE;

  switch (nec_state) {
  case NEC_STATE_IDLE:
    if (pulse_us > NEC_HDR_MARK_MIN && pulse_us < NEC_HDR_MARK_MAX) {
      nec_state = NEC_STATE_LEAD_SPACE;
    }
    break;

  case NEC_STATE_LEAD_SPACE:
    if (pulse_us > NEC_HDR_SPACE_MIN && pulse_us < NEC_HDR_SPACE_MAX) {
      nec_data = 0;
      bit_index = 0;
      nec_state = NEC_STATE_BIT_MARK;
    } else {
      nec_state = NEC_STATE_IDLE;
    }
    break;

  case NEC_STATE_BIT_MARK:
    if (pulse_us > NEC_BIT_MARK_MIN && pulse_us < NEC_BIT_MARK_MAX) {
      nec_state = NEC_STATE_BIT_SPACE;
    } else {
      nec_state = NEC_STATE_IDLE;
    }
    break;

  case NEC_STATE_BIT_SPACE:
    if ((pulse_us > NEC_ONE_SPACE_MIN) && (pulse_us < NEC_ONE_SPACE_MAX)) {
      nec_data |= (1UL << bit_index);
    } else if ((pulse_us > NEC_ZERO_SPACE_MIN) &&
               (pulse_us < NEC_ZERO_SPACE_MAX)) {
      nec_data &= ~(1UL << bit_index);
    } else {
      nec_state = NEC_STATE_IDLE;
      break;
    }

    bit_index++;
    if (bit_index >= NEC_BITS) {
      last_cmd =
          (blfm_ir_command_t)((nec_data & NEC_CMD_MASK) >> NEC_CMD_SHIFT);
      nec_state = NEC_STATE_IDLE;
    } else {
      nec_state = NEC_STATE_BIT_MARK;
    }
    break;

  default:
    nec_state = NEC_STATE_IDLE;
    break;
  }

  return last_cmd;
}

static blfm_ir_command_t decode_ir_code(uint32_t pulse_us) {
  return blfm_ir_remote_process_pulse(pulse_us);
}

void ir_exti_handler(void) {
  uint32_t now = DWT->CYCCNT;
  uint32_t diff = now - last_edge_time;
  last_edge_time = now;

  uint32_t pulse_us = (diff * 1000000UL) / SystemCoreClock;

  //volatile uint32_t debug_pulse = pulse_us;

  // Read logic level at the pin (PA8)
  bool is_high = (GPIOA->IDR & GPIO_IDR_IDR8) != 0;

  // Only process falling edge as meaningful pulse end
  // (Optionally, you can process both edges if you're decoding both
  // marks/spaces separately)
  if (!is_high) {
    blfm_ir_command_t cmd = decode_ir_code(pulse_us);

    if (ir_controller_queue != NULL) {
      blfm_ir_remote_event_t event = {
          .timestamp = xTaskGetTickCountFromISR(),
          .pulse_us = pulse_us, // Don't mask this
          .command = cmd,
      };
      BaseType_t hpTaskWoken = pdFALSE;
      xQueueSendFromISR(ir_controller_queue, &event, &hpTaskWoken);
      portYIELD_FROM_ISR(hpTaskWoken);
    }
  }

  // Clear EXTI line 8 pending bit if you're manually managing it
  // (depends on your EXTI handling setup)
  // EXTI->PR = EXTI_PR_PR8;
}

void blfm_ir_remote_init(QueueHandle_t controller_queue) {
  ir_controller_queue = controller_queue;

  // Enable GPIOA and AFIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // Configure PA8 as input with pull-up
  GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
  GPIOA->CRH |= GPIO_CRH_CNF8_1; // Input, pull-up/pull-down
  GPIOA->ODR |= GPIO_ODR_ODR8;   // Pull-up enabled

  // Map EXTI8 to PA8
  AFIO->EXTICR[2] &= ~AFIO_EXTICR3_EXTI8;
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA;

  // Unmask EXTI line 8
  EXTI->IMR |= EXTI_IMR_MR8;

  // Enable both rising and falling edge triggers
  EXTI->RTSR |= EXTI_RTSR_TR8; // Rising
  EXTI->FTSR |= EXTI_FTSR_TR8; // Falling

  // Register and enable EXTI IRQ
  blfm_exti_register_callback(8, ir_exti_handler);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}
