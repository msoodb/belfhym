/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_CAN_H
#define BLFM_CAN_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief CAN frame type enumeration
 */
typedef enum {
  BLFM_CAN_FRAME_STANDARD = 0,
  BLFM_CAN_FRAME_EXTENDED
} blfm_can_frame_type_t;

/**
 * @brief CAN frame format enumeration
 */
typedef enum {
  BLFM_CAN_FRAME_DATA = 0,
  BLFM_CAN_FRAME_REMOTE
} blfm_can_frame_format_t;

/**
 * @brief CAN message structure
 */
typedef struct {
  uint32_t id;                    // Message ID (11-bit standard, 29-bit extended)
  blfm_can_frame_type_t type;     // Standard or Extended frame
  blfm_can_frame_format_t format; // Data or Remote frame
  uint8_t dlc;                    // Data Length Code (0-8)
  uint8_t data[8];                // Data bytes
} blfm_can_message_t;

/**
 * @brief CAN bit timing configuration
 */
typedef struct {
  uint16_t prescaler;             // Baud rate prescaler (1-1024)
  uint8_t time_segment1;          // Time segment 1 (1-16)
  uint8_t time_segment2;          // Time segment 2 (1-8)
  uint8_t sync_jump_width;        // Synchronization jump width (1-4)
} blfm_can_bit_timing_t;

/**
 * @brief CAN configuration structure
 */
typedef struct {
  blfm_can_bit_timing_t bit_timing;
  bool loopback_mode;             // Loopback mode for testing
  bool silent_mode;               // Silent mode (listen only)
  bool auto_retransmission;       // Automatic retransmission
  bool receive_fifo_locked;       // Receive FIFO locked mode
  bool transmit_fifo_priority;    // Transmit FIFO priority mode
  uint32_t timeout_ms;
} blfm_can_config_t;

/**
 * @brief CAN filter configuration
 */
typedef struct {
  uint32_t id;                    // Filter ID
  uint32_t mask;                  // Filter mask
  blfm_can_frame_type_t type;     // Standard or Extended
  uint8_t fifo;                   // FIFO assignment (0 or 1)
  bool active;                    // Filter active
} blfm_can_filter_t;

/**
 * @brief CAN error codes
 */
typedef enum {
  BLFM_CAN_OK = 0,
  BLFM_CAN_ERR_INVALID_CONFIG,
  BLFM_CAN_ERR_NOT_INITIALIZED,
  BLFM_CAN_ERR_TIMEOUT,
  BLFM_CAN_ERR_NULL_PTR,
  BLFM_CAN_ERR_BUSY,
  BLFM_CAN_ERR_FULL,
  BLFM_CAN_ERR_EMPTY,
  BLFM_CAN_ERR_FILTER
} blfm_can_error_t;

/**
 * @brief CAN status enumeration
 */
typedef enum {
  BLFM_CAN_STATUS_ERROR_ACTIVE = 0,
  BLFM_CAN_STATUS_ERROR_PASSIVE,
  BLFM_CAN_STATUS_BUS_OFF
} blfm_can_status_t;

/**
 * @brief Initialize CAN peripheral
 * @param config Configuration structure (can be NULL for default)
 * @return Error code
 */
blfm_can_error_t blfm_can_init(const blfm_can_config_t *config);

/**
 * @brief Deinitialize CAN peripheral
 * @return Error code
 */
blfm_can_error_t blfm_can_deinit(void);

/**
 * @brief Configure CAN filter
 * @param filter_num Filter number (0-13)
 * @param filter Filter configuration
 * @return Error code
 */
blfm_can_error_t blfm_can_configure_filter(uint8_t filter_num, const blfm_can_filter_t *filter);

/**
 * @brief Transmit CAN message
 * @param message Message to transmit
 * @return Error code
 */
blfm_can_error_t blfm_can_transmit(const blfm_can_message_t *message);

/**
 * @brief Receive CAN message
 * @param message Buffer to store received message
 * @param fifo FIFO number (0 or 1)
 * @return Error code
 */
blfm_can_error_t blfm_can_receive(blfm_can_message_t *message, uint8_t fifo);

/**
 * @brief Check if message is pending in receive FIFO
 * @param fifo FIFO number (0 or 1)
 * @return Number of pending messages
 */
uint8_t blfm_can_messages_pending(uint8_t fifo);

/**
 * @brief Check if transmit mailbox is empty
 * @return true if any mailbox is empty
 */
bool blfm_can_transmit_ready(void);

/**
 * @brief Get CAN status
 * @return CAN status
 */
blfm_can_status_t blfm_can_get_status(void);

/**
 * @brief Get error counters
 * @param tx_error_count Pointer to store TX error count
 * @param rx_error_count Pointer to store RX error count
 * @return Error code
 */
blfm_can_error_t blfm_can_get_error_counters(uint8_t *tx_error_count, uint8_t *rx_error_count);

/**
 * @brief Standard bit timing presets for common baud rates
 */
extern const blfm_can_bit_timing_t BLFM_CAN_TIMING_125K;   // 125 kbps
extern const blfm_can_bit_timing_t BLFM_CAN_TIMING_250K;   // 250 kbps
extern const blfm_can_bit_timing_t BLFM_CAN_TIMING_500K;   // 500 kbps
extern const blfm_can_bit_timing_t BLFM_CAN_TIMING_1M;     // 1 Mbps

#endif // BLFM_CAN_H