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

#define NEC_START_LOW_MIN 1000
#define NEC_START_LOW_MAX 10000
#define NEC_START_HIGH_MIN 1000
#define NEC_START_HIGH_MAX 5000

#define NEC_BIT_LOW_MIN 300
#define NEC_BIT_LOW_MAX 1000
#define NEC_BIT1_HIGH_MIN 1000
#define NEC_BIT1_HIGH_MAX 2000
#define NEC_BIT0_HIGH_MIN 300
#define NEC_BIT0_HIGH_MAX 1000

typedef enum {
  NEC_STATE_IDLE,
  NEC_STATE_LEAD_HIGH,
  NEC_STATE_BIT_LOW,
  NEC_STATE_BIT_HIGH
} nec_state_t;

static nec_state_t nec_state = NEC_STATE_IDLE;
static uint32_t nec_data = 0;
static uint8_t bit_index = 0;
static uint32_t last_edge_time = 0;
static uint8_t decoded_command = 0xFF;

static QueueHandle_t ir_controller_queue = NULL;

uint8_t get_ir_raw_code(void) { return decoded_command; }

static bool pulse_in_range(uint32_t width, uint32_t min, uint32_t max) {
  return (width >= min) && (width <= max);
}

static blfm_ir_command_t decode_ir_code(uint32_t pulse_us) {
  switch (pulse_us) {
  case 0x45:
    return BLFM_IR_CMD_SET_AUTO;
  case 0x46:
    return BLFM_IR_CMD_SET_MANUAL;
  case 0xA2:
    return BLFM_IR_CMD_TOGGLE_POWER;
  case 0x62:
    return BLFM_IR_CMD_FORWARD;
  case 0x22:
    return BLFM_IR_CMD_BACKWARD;
  case 0xC2:
    return BLFM_IR_CMD_LEFT;
  case 0xE2:
    return BLFM_IR_CMD_RIGHT;
  default:
    return BLFM_IR_CMD_NONE;
  }
}

void ir_exti_handler(void) {
  uint32_t now = DWT->CYCCNT;
  uint32_t diff = now - last_edge_time;
  last_edge_time = now;
  uint32_t pulse_us = (diff * 1000000UL) / SystemCoreClock;

  blfm_ir_command_t cmd = decode_ir_code(pulse_us);
  
  if (ir_controller_queue != NULL) {
    blfm_ir_remote_event_t event = {
      .timestamp = xTaskGetTickCountFromISR(),
      .pulse_us = (uint8_t)(pulse_us & 0xFF),
      .command = cmd,
    };
    BaseType_t hpTaskWoken = pdFALSE;
    xQueueSendFromISR(ir_controller_queue, &event, &hpTaskWoken);
    portYIELD_FROM_ISR(hpTaskWoken);
  }
}

void blfm_ir_remote_init(QueueHandle_t controller_queue) {
  ir_controller_queue = controller_queue;

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // Configure PA8 as input pull-up
  GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
  GPIOA->CRH |= GPIO_CRH_CNF8_1;
  GPIOA->ODR |= GPIO_ODR_ODR8;

  // Map EXTI8 to PA8
  AFIO->EXTICR[2] &= ~AFIO_EXTICR3_EXTI8;
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA;

  EXTI->IMR |= EXTI_IMR_MR8;
  // Configure EXTI8: falling edge trigger
  EXTI->FTSR |= EXTI_FTSR_TR8;

  blfm_exti_register_callback(8, ir_exti_handler);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}
