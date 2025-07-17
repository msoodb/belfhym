/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_can.h"
#include "blfm_pins.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"
#include <stddef.h>

#define CAN_DEFAULT_TIMEOUT_MS 100

/**
 * @brief Standard bit timing presets for 72MHz APB1 clock
 */
const blfm_can_bit_timing_t BLFM_CAN_TIMING_125K = {
  .prescaler = 18,
  .time_segment1 = 13,
  .time_segment2 = 2,
  .sync_jump_width = 1
};

const blfm_can_bit_timing_t BLFM_CAN_TIMING_250K = {
  .prescaler = 9,
  .time_segment1 = 13,
  .time_segment2 = 2,
  .sync_jump_width = 1
};

const blfm_can_bit_timing_t BLFM_CAN_TIMING_500K = {
  .prescaler = 4,
  .time_segment1 = 15,
  .time_segment2 = 2,
  .sync_jump_width = 1
};

const blfm_can_bit_timing_t BLFM_CAN_TIMING_1M = {
  .prescaler = 2,
  .time_segment1 = 15,
  .time_segment2 = 2,
  .sync_jump_width = 1
};

static uint32_t can_timeout_counter = 10000;
static bool can_initialized = false;

/**
 * @brief Wait for CAN flag with timeout
 * @param flag Flag to wait for
 * @param state Expected flag state
 * @return BLFM_CAN_OK on success, BLFM_CAN_ERR_TIMEOUT on timeout
 */
static blfm_can_error_t blfm_can_wait_flag(uint32_t flag, bool state) {
  volatile uint32_t timeout = can_timeout_counter;
  while (((CAN1->MSR & flag) != 0) != state) {
    if (--timeout == 0) {
      return BLFM_CAN_ERR_TIMEOUT;
    }
  }
  return BLFM_CAN_OK;
}

/**
 * @brief Configure GPIO pins for CAN
 */
static void blfm_can_configure_gpio(void) {
  // Enable GPIO clocks
  if (BLFM_CAN_TX_PORT == GPIOA) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  } else if (BLFM_CAN_TX_PORT == GPIOB) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  }
  
  // Enable alternate function clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Configure CAN TX as alternate function push-pull
  blfm_gpio_config_alternate_pushpull((uint32_t)BLFM_CAN_TX_PORT, BLFM_CAN_TX_PIN);
  
  // Configure CAN RX as input floating
  blfm_gpio_config_input((uint32_t)BLFM_CAN_RX_PORT, BLFM_CAN_RX_PIN);
}

/**
 * @brief Find empty transmit mailbox
 * @return Mailbox number (0-2) or -1 if none available
 */
static int8_t blfm_can_find_empty_mailbox(void) {
  uint32_t tsr = CAN1->TSR;
  if (tsr & CAN_TSR_TME0) return 0;
  if (tsr & CAN_TSR_TME1) return 1;
  if (tsr & CAN_TSR_TME2) return 2;
  return -1;
}

blfm_can_error_t blfm_can_init(const blfm_can_config_t *config) {
  // Use default config if none provided
  blfm_can_config_t default_config = {
    .bit_timing = BLFM_CAN_TIMING_500K,
    .loopback_mode = false,
    .silent_mode = false,
    .auto_retransmission = true,
    .receive_fifo_locked = false,
    .transmit_fifo_priority = false,
    .timeout_ms = CAN_DEFAULT_TIMEOUT_MS
  };
  
  if (config == NULL) {
    config = &default_config;
  }

  // Update timeout counter
  can_timeout_counter = config->timeout_ms * 100;

  // Enable CAN peripheral clock
  RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

  // Configure GPIO pins
  blfm_can_configure_gpio();

  // Request initialization mode
  CAN1->MCR |= CAN_MCR_INRQ;
  
  // Wait for initialization mode
  blfm_can_error_t result = blfm_can_wait_flag(CAN_MSR_INAK, true);
  if (result != BLFM_CAN_OK) {
    return result;
  }

  // Configure bit timing
  uint32_t btr = 0;
  btr |= (config->bit_timing.sync_jump_width - 1) << 24;
  btr |= (config->bit_timing.time_segment1 - 1) << 16;
  btr |= (config->bit_timing.time_segment2 - 1) << 20;
  btr |= (config->bit_timing.prescaler - 1);
  
  // Set mode flags
  if (config->loopback_mode) {
    btr |= CAN_BTR_LBKM;
  }
  if (config->silent_mode) {
    btr |= CAN_BTR_SILM;
  }
  
  CAN1->BTR = btr;

  // Configure MCR
  uint32_t mcr = CAN_MCR_ABOM;  // Automatic bus-off management
  
  if (!config->auto_retransmission) {
    mcr |= CAN_MCR_NART;
  }
  if (config->receive_fifo_locked) {
    mcr |= CAN_MCR_RFLM;
  }
  if (config->transmit_fifo_priority) {
    mcr |= CAN_MCR_TXFP;
  }
  
  CAN1->MCR = mcr;

  // Leave initialization mode
  CAN1->MCR &= ~CAN_MCR_INRQ;
  
  // Wait for normal mode
  result = blfm_can_wait_flag(CAN_MSR_INAK, false);
  if (result != BLFM_CAN_OK) {
    return result;
  }

  can_initialized = true;
  return BLFM_CAN_OK;
}

blfm_can_error_t blfm_can_deinit(void) {
  // Request initialization mode
  CAN1->MCR |= CAN_MCR_INRQ;
  
  // Wait for initialization mode
  blfm_can_error_t result = blfm_can_wait_flag(CAN_MSR_INAK, true);
  if (result != BLFM_CAN_OK) {
    return result;
  }

  // Disable CAN peripheral
  CAN1->MCR = CAN_MCR_RESET;
  
  // Disable peripheral clock
  RCC->APB1ENR &= ~RCC_APB1ENR_CAN1EN;

  can_initialized = false;
  return BLFM_CAN_OK;
}

blfm_can_error_t blfm_can_configure_filter(uint8_t filter_num, const blfm_can_filter_t *filter) {
  if (filter_num > 13) {
    return BLFM_CAN_ERR_FILTER;
  }
  if (filter == NULL) {
    return BLFM_CAN_ERR_NULL_PTR;
  }

  // Enter filter initialization mode
  CAN1->FMR |= CAN_FMR_FINIT;

  uint32_t filter_bit = 1 << filter_num;

  if (filter->active) {
    // Configure filter
    if (filter->type == BLFM_CAN_FRAME_EXTENDED) {
      // Extended frame - use mask mode
      CAN1->FM1R &= ~filter_bit;  // Mask mode
      CAN1->FS1R |= filter_bit;   // 32-bit scale
      
      CAN1->sFilterRegister[filter_num].FR1 = (filter->id << 3) | CAN_RI0R_IDE;
      CAN1->sFilterRegister[filter_num].FR2 = (filter->mask << 3) | CAN_RI0R_IDE;
    } else {
      // Standard frame - use mask mode
      CAN1->FM1R &= ~filter_bit;  // Mask mode
      CAN1->FS1R &= ~filter_bit;  // 16-bit scale
      
      CAN1->sFilterRegister[filter_num].FR1 = (filter->id << 21) | (filter->mask << 5);
      CAN1->sFilterRegister[filter_num].FR2 = 0;
    }

    // Set FIFO assignment
    if (filter->fifo == 1) {
      CAN1->FFA1R |= filter_bit;
    } else {
      CAN1->FFA1R &= ~filter_bit;
    }

    // Activate filter
    CAN1->FA1R |= filter_bit;
  } else {
    // Deactivate filter
    CAN1->FA1R &= ~filter_bit;
  }

  // Leave filter initialization mode
  CAN1->FMR &= ~CAN_FMR_FINIT;

  return BLFM_CAN_OK;
}

blfm_can_error_t blfm_can_transmit(const blfm_can_message_t *message) {
  if (!can_initialized) {
    return BLFM_CAN_ERR_NOT_INITIALIZED;
  }
  if (message == NULL) {
    return BLFM_CAN_ERR_NULL_PTR;
  }
  if (message->dlc > 8) {
    return BLFM_CAN_ERR_INVALID_CONFIG;
  }

  // Find empty mailbox
  int8_t mailbox = blfm_can_find_empty_mailbox();
  if (mailbox < 0) {
    return BLFM_CAN_ERR_BUSY;
  }

  // Configure transmit mailbox
  CAN_TxMailBox_TypeDef *tx_mailbox = &CAN1->sTxMailBox[mailbox];

  // Set identifier
  if (message->type == BLFM_CAN_FRAME_EXTENDED) {
    tx_mailbox->TIR = (message->id << 3) | CAN_TI0R_IDE;
  } else {
    tx_mailbox->TIR = message->id << 21;
  }

  // Set frame type
  if (message->format == BLFM_CAN_FRAME_REMOTE) {
    tx_mailbox->TIR |= CAN_TI0R_RTR;
  }

  // Set data length
  tx_mailbox->TDTR = message->dlc;

  // Set data
  tx_mailbox->TDLR = (message->data[3] << 24) | (message->data[2] << 16) | 
                     (message->data[1] << 8) | message->data[0];
  tx_mailbox->TDHR = (message->data[7] << 24) | (message->data[6] << 16) | 
                     (message->data[5] << 8) | message->data[4];

  // Request transmission
  tx_mailbox->TIR |= CAN_TI0R_TXRQ;

  return BLFM_CAN_OK;
}

blfm_can_error_t blfm_can_receive(blfm_can_message_t *message, uint8_t fifo) {
  if (!can_initialized) {
    return BLFM_CAN_ERR_NOT_INITIALIZED;
  }
  if (message == NULL) {
    return BLFM_CAN_ERR_NULL_PTR;
  }
  if (fifo > 1) {
    return BLFM_CAN_ERR_INVALID_CONFIG;
  }

  // Check if message is pending
  if (blfm_can_messages_pending(fifo) == 0) {
    return BLFM_CAN_ERR_EMPTY;
  }

  CAN_FIFOMailBox_TypeDef *rx_mailbox = &CAN1->sFIFOMailBox[fifo];

  // Get identifier and frame type
  uint32_t rir = rx_mailbox->RIR;
  if (rir & CAN_RI0R_IDE) {
    message->type = BLFM_CAN_FRAME_EXTENDED;
    message->id = rir >> 3;
  } else {
    message->type = BLFM_CAN_FRAME_STANDARD;
    message->id = rir >> 21;
  }

  // Get frame format
  message->format = (rir & CAN_RI0R_RTR) ? BLFM_CAN_FRAME_REMOTE : BLFM_CAN_FRAME_DATA;

  // Get data length
  message->dlc = rx_mailbox->RDTR & 0x0F;

  // Get data
  uint32_t rdlr = rx_mailbox->RDLR;
  uint32_t rdhr = rx_mailbox->RDHR;
  
  message->data[0] = rdlr & 0xFF;
  message->data[1] = (rdlr >> 8) & 0xFF;
  message->data[2] = (rdlr >> 16) & 0xFF;
  message->data[3] = (rdlr >> 24) & 0xFF;
  message->data[4] = rdhr & 0xFF;
  message->data[5] = (rdhr >> 8) & 0xFF;
  message->data[6] = (rdhr >> 16) & 0xFF;
  message->data[7] = (rdhr >> 24) & 0xFF;

  // Release FIFO
  if (fifo == 0) {
    CAN1->RF0R |= CAN_RF0R_RFOM0;
  } else {
    CAN1->RF1R |= CAN_RF1R_RFOM1;
  }

  return BLFM_CAN_OK;
}

uint8_t blfm_can_messages_pending(uint8_t fifo) {
  if (!can_initialized || fifo > 1) {
    return 0;
  }

  if (fifo == 0) {
    return CAN1->RF0R & CAN_RF0R_FMP0;
  } else {
    return CAN1->RF1R & CAN_RF1R_FMP1;
  }
}

bool blfm_can_transmit_ready(void) {
  if (!can_initialized) {
    return false;
  }
  return (CAN1->TSR & (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2)) != 0;
}

blfm_can_status_t blfm_can_get_status(void) {
  if (!can_initialized) {
    return BLFM_CAN_STATUS_BUS_OFF;
  }

  uint32_t esr = CAN1->ESR;
  
  if (esr & CAN_ESR_BOFF) {
    return BLFM_CAN_STATUS_BUS_OFF;
  } else if (esr & CAN_ESR_EPVF) {
    return BLFM_CAN_STATUS_ERROR_PASSIVE;
  } else {
    return BLFM_CAN_STATUS_ERROR_ACTIVE;
  }
}

blfm_can_error_t blfm_can_get_error_counters(uint8_t *tx_error_count, uint8_t *rx_error_count) {
  if (!can_initialized) {
    return BLFM_CAN_ERR_NOT_INITIALIZED;
  }
  if (tx_error_count == NULL || rx_error_count == NULL) {
    return BLFM_CAN_ERR_NULL_PTR;
  }

  uint32_t esr = CAN1->ESR;
  *tx_error_count = (esr & CAN_ESR_TEC) >> 16;
  *rx_error_count = (esr & CAN_ESR_REC) >> 24;

  return BLFM_CAN_OK;
}