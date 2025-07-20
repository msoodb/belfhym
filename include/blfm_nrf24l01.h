/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_NRF24L01_H
#define BLFM_NRF24L01_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "blfm_types.h"

// =============================================================================
// NRF24L01+ Register Definitions
// =============================================================================

// Register Map
#define NRF24_REG_CONFIG        0x00  // Configuration Register
#define NRF24_REG_EN_AA         0x01  // Enable Auto Acknowledgment
#define NRF24_REG_EN_RXADDR     0x02  // Enabled RX Addresses
#define NRF24_REG_SETUP_AW      0x03  // Setup Address Widths
#define NRF24_REG_SETUP_RETR    0x04  // Setup Auto. Retrans
#define NRF24_REG_RF_CH         0x05  // RF Channel
#define NRF24_REG_RF_SETUP      0x06  // RF Setup Register
#define NRF24_REG_STATUS        0x07  // Status Register
#define NRF24_REG_OBSERVE_TX    0x08  // Observe TX
#define NRF24_REG_RPD           0x09  // Received Power Detector
#define NRF24_REG_RX_ADDR_P0    0x0A  // RX address pipe0
#define NRF24_REG_RX_ADDR_P1    0x0B  // RX address pipe1
#define NRF24_REG_RX_ADDR_P2    0x0C  // RX address pipe2
#define NRF24_REG_RX_ADDR_P3    0x0D  // RX address pipe3
#define NRF24_REG_RX_ADDR_P4    0x0E  // RX address pipe4
#define NRF24_REG_RX_ADDR_P5    0x0F  // RX address pipe5
#define NRF24_REG_TX_ADDR       0x10  // TX address
#define NRF24_REG_RX_PW_P0      0x11  // RX payload width, pipe0
#define NRF24_REG_RX_PW_P1      0x12  // RX payload width, pipe1
#define NRF24_REG_RX_PW_P2      0x13  // RX payload width, pipe2
#define NRF24_REG_RX_PW_P3      0x14  // RX payload width, pipe3
#define NRF24_REG_RX_PW_P4      0x15  // RX payload width, pipe4
#define NRF24_REG_RX_PW_P5      0x16  // RX payload width, pipe5
#define NRF24_REG_FIFO_STATUS   0x17  // FIFO Status Register
#define NRF24_REG_DYNPD         0x1C  // Enable dynamic payload length
#define NRF24_REG_FEATURE       0x1D  // Feature Register

// Commands
#define NRF24_CMD_R_REGISTER    0x00  // Read command and status registers
#define NRF24_CMD_W_REGISTER    0x20  // Write command and status registers
#define NRF24_CMD_R_RX_PAYLOAD  0x61  // Read RX payload
#define NRF24_CMD_W_TX_PAYLOAD  0xA0  // Write TX payload
#define NRF24_CMD_FLUSH_TX      0xE1  // Flush TX FIFO
#define NRF24_CMD_FLUSH_RX      0xE2  // Flush RX FIFO
#define NRF24_CMD_REUSE_TX_PL   0xE3  // Reuse last transmitted payload
#define NRF24_CMD_R_RX_PL_WID   0x60  // Read RX payload width for top R_RX_PAYLOAD
#define NRF24_CMD_W_ACK_PAYLOAD 0xA8  // Write Payload to be transmitted with ACK
#define NRF24_CMD_W_TX_PAYLOAD_NO_ACK 0xB0  // Disables AUTOACK on this specific packet
#define NRF24_CMD_NOP           0xFF  // No Operation

// Config Register Bits
#define NRF24_CONFIG_MASK_RX_DR  0x40  // Mask interrupt caused by RX_DR
#define NRF24_CONFIG_MASK_TX_DS  0x20  // Mask interrupt caused by TX_DS
#define NRF24_CONFIG_MASK_MAX_RT 0x10  // Mask interrupt caused by MAX_RT
#define NRF24_CONFIG_EN_CRC      0x08  // Enable CRC
#define NRF24_CONFIG_CRCO        0x04  // CRC encoding scheme (0=1 byte, 1=2 bytes)
#define NRF24_CONFIG_PWR_UP      0x02  // Power up
#define NRF24_CONFIG_PRIM_RX     0x01  // RX/TX control (0=PTX, 1=PRX)

// Status Register Bits
#define NRF24_STATUS_RX_DR       0x40  // Data Ready RX FIFO interrupt
#define NRF24_STATUS_TX_DS       0x20  // Data Sent TX FIFO interrupt
#define NRF24_STATUS_MAX_RT      0x10  // Maximum number of TX retransmits interrupt
#define NRF24_STATUS_RX_P_NO     0x0E  // Data pipe number for payload (bit 3:1)
#define NRF24_STATUS_TX_FULL     0x01  // TX FIFO full flag

// RF Setup Register Bits
#define NRF24_RF_SETUP_CONT_WAVE 0x80  // Continuous carrier transmit
#define NRF24_RF_SETUP_RF_DR_LOW 0x20  // Set RF Data Rate to 250kbps
#define NRF24_RF_SETUP_PLL_LOCK  0x10  // Force PLL lock signal
#define NRF24_RF_SETUP_RF_DR_HIGH 0x08 // Set RF Data Rate to 2Mbps
#define NRF24_RF_SETUP_RF_PWR    0x06  // Set RF output power (bit 2:1)

// FIFO Status Register Bits
#define NRF24_FIFO_STATUS_TX_REUSE 0x40  // Reuse last transmitted data packet
#define NRF24_FIFO_STATUS_TX_FULL  0x20  // TX FIFO full flag
#define NRF24_FIFO_STATUS_TX_EMPTY 0x10  // TX FIFO empty flag
#define NRF24_FIFO_STATUS_RX_FULL  0x02  // RX FIFO full flag
#define NRF24_FIFO_STATUS_RX_EMPTY 0x01  // RX FIFO empty flag

// =============================================================================
// Configuration Constants
// =============================================================================

#define NRF24_MAX_PAYLOAD_SIZE   32   // Maximum payload size
#define NRF24_ADDR_WIDTH         5    // Address width in bytes
#define NRF24_MAX_CHANNEL        125  // Maximum RF channel
#define NRF24_DEFAULT_CHANNEL    76   // Default RF channel (2476 MHz)

// Timing Constants (microseconds)
#define NRF24_POWER_UP_DELAY_US     5000   // Power up delay
#define NRF24_CE_PULSE_WIDTH_US     15     // CE pulse width for TX
#define NRF24_TX_SETTLE_US          130    // TX settling time
#define NRF24_RX_SETTLE_US          130    // RX settling time

// =============================================================================
// Data Types
// =============================================================================

/**
 * @brief NRF24L01 data rates
 */
typedef enum {
    NRF24_DATA_RATE_250K = 0,    // 250 kbps
    NRF24_DATA_RATE_1M,          // 1 Mbps
    NRF24_DATA_RATE_2M           // 2 Mbps
} blfm_nrf24_data_rate_t;

/**
 * @brief NRF24L01 power levels
 */
typedef enum {
    NRF24_POWER_MINUS_18_DBM = 0,  // -18 dBm
    NRF24_POWER_MINUS_12_DBM,      // -12 dBm
    NRF24_POWER_MINUS_6_DBM,       // -6 dBm
    NRF24_POWER_0_DBM              // 0 dBm
} blfm_nrf24_power_t;

/**
 * @brief NRF24L01 CRC length
 */
typedef enum {
    NRF24_CRC_DISABLED = 0,
    NRF24_CRC_1_BYTE,
    NRF24_CRC_2_BYTE
} blfm_nrf24_crc_t;

/**
 * @brief NRF24L01 operation mode
 */
typedef enum {
    NRF24_MODE_POWER_DOWN = 0,
    NRF24_MODE_STANDBY,
    NRF24_MODE_TX,
    NRF24_MODE_RX
} blfm_nrf24_mode_t;

/**
 * @brief NRF24L01 pipe configuration
 */
typedef struct {
    uint8_t address[NRF24_ADDR_WIDTH];
    uint8_t payload_size;
    bool auto_ack;
    bool enabled;
} blfm_nrf24_pipe_config_t;

/**
 * @brief NRF24L01 configuration structure
 */
typedef struct {
    uint8_t channel;                           // RF channel (0-125)
    blfm_nrf24_data_rate_t data_rate;         // Data rate
    blfm_nrf24_power_t power;                 // TX power
    blfm_nrf24_crc_t crc;                     // CRC configuration
    uint8_t auto_retransmit_count;            // Auto retransmit count (0-15)
    uint8_t auto_retransmit_delay;            // Auto retransmit delay (0-15)
    blfm_nrf24_pipe_config_t pipes[6];        // Pipe configurations
    uint8_t tx_address[NRF24_ADDR_WIDTH];     // TX address
} blfm_nrf24_config_t;

/**
 * @brief NRF24L01 status information
 */
typedef struct {
    bool data_ready;
    bool data_sent;
    bool max_retransmit;
    uint8_t rx_pipe_number;
    bool tx_fifo_full;
    bool rx_fifo_full;
    bool rx_fifo_empty;
    bool tx_fifo_empty;
    uint8_t lost_packets;
    uint8_t retransmit_count;
} blfm_nrf24_status_t;

/**
 * @brief NRF24L01 packet structure
 */
typedef struct {
    uint8_t data[NRF24_MAX_PAYLOAD_SIZE];
    uint8_t size;
    uint8_t pipe;
    TickType_t timestamp;
} blfm_nrf24_packet_t;

/**
 * @brief NRF24L01 result codes
 */
typedef enum {
    NRF24_SUCCESS = 0,
    NRF24_ERROR_INVALID_PARAM,
    NRF24_ERROR_SPI_TIMEOUT,
    NRF24_ERROR_NOT_INITIALIZED,
    NRF24_ERROR_TX_TIMEOUT,
    NRF24_ERROR_RX_TIMEOUT,
    NRF24_ERROR_FIFO_FULL,
    NRF24_ERROR_HARDWARE
} blfm_nrf24_result_t;

// =============================================================================
// Function Prototypes
// =============================================================================

/**
 * @brief Initialize NRF24L01 module
 * @param config Pointer to configuration structure
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_init(const blfm_nrf24_config_t *config);

/**
 * @brief Deinitialize NRF24L01 module
 */
void blfm_nrf24_deinit(void);

/**
 * @brief Set operation mode
 * @param mode Target operation mode
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_set_mode(blfm_nrf24_mode_t mode);

/**
 * @brief Get current operation mode
 * @return Current operation mode
 */
blfm_nrf24_mode_t blfm_nrf24_get_mode(void);

/**
 * @brief Set RF channel
 * @param channel RF channel (0-125)
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_set_channel(uint8_t channel);

/**
 * @brief Get RF channel
 * @return Current RF channel
 */
uint8_t blfm_nrf24_get_channel(void);

/**
 * @brief Set TX address
 * @param address Pointer to 5-byte address
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_set_tx_address(const uint8_t *address);

/**
 * @brief Set RX address for specific pipe
 * @param pipe Pipe number (0-5)
 * @param address Pointer to address (5 bytes for pipe 0-1, 1 byte for pipe 2-5)
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_set_rx_address(uint8_t pipe, const uint8_t *address);

/**
 * @brief Enable/disable RX pipe
 * @param pipe Pipe number (0-5)
 * @param enable Enable flag
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_enable_pipe(uint8_t pipe, bool enable);

/**
 * @brief Set payload size for specific pipe
 * @param pipe Pipe number (0-5)
 * @param size Payload size (1-32 bytes)
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_set_payload_size(uint8_t pipe, uint8_t size);

/**
 * @brief Send data packet
 * @param data Pointer to data buffer
 * @param size Data size (1-32 bytes)
 * @param timeout_ms Timeout in milliseconds
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_send(const uint8_t *data, uint8_t size, uint32_t timeout_ms);

/**
 * @brief Receive data packet (non-blocking)
 * @param packet Pointer to packet structure to fill
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_receive(blfm_nrf24_packet_t *packet);

/**
 * @brief Check if data is available
 * @return True if data is available
 */
bool blfm_nrf24_data_available(void);

/**
 * @brief Check if data is available on specific pipe
 * @param pipe Pipe number to check
 * @return True if data is available on specified pipe
 */
bool blfm_nrf24_data_available_pipe(uint8_t pipe);

/**
 * @brief Get status information
 * @param status Pointer to status structure to fill
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_get_status(blfm_nrf24_status_t *status);

/**
 * @brief Clear status flags
 * @param flags Flags to clear (combination of NRF24_STATUS_* bits)
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_clear_status(uint8_t flags);

/**
 * @brief Flush TX FIFO
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_flush_tx(void);

/**
 * @brief Flush RX FIFO
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_flush_rx(void);

/**
 * @brief Power up the module
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_power_up(void);

/**
 * @brief Power down the module
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_power_down(void);

/**
 * @brief Test if module is connected and responding
 * @return True if module is detected
 */
bool blfm_nrf24_test_connection(void);

/**
 * @brief Get default configuration
 * @param config Pointer to configuration structure to fill
 */
void blfm_nrf24_get_default_config(blfm_nrf24_config_t *config);

// =============================================================================
// Low-level Register Access (for advanced usage)
// =============================================================================

/**
 * @brief Read register value
 * @param reg Register address
 * @return Register value
 */
uint8_t blfm_nrf24_read_register(uint8_t reg);

/**
 * @brief Write register value
 * @param reg Register address
 * @param value Value to write
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_write_register(uint8_t reg, uint8_t value);

/**
 * @brief Read multiple bytes from register
 * @param reg Register address
 * @param data Pointer to data buffer
 * @param len Number of bytes to read
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_read_register_multi(uint8_t reg, uint8_t *data, uint8_t len);

/**
 * @brief Write multiple bytes to register
 * @param reg Register address
 * @param data Pointer to data buffer
 * @param len Number of bytes to write
 * @return Result code
 */
blfm_nrf24_result_t blfm_nrf24_write_register_multi(uint8_t reg, const uint8_t *data, uint8_t len);

#endif /* BLFM_NRF24L01_H */