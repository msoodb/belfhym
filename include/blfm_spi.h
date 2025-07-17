/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_SPI_H
#define BLFM_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief SPI peripheral enumeration
 */
typedef enum {
  BLFM_SPI1 = 0,
  BLFM_SPI2,
  BLFM_SPI_COUNT
} blfm_spi_peripheral_t;

/**
 * @brief SPI mode enumeration
 */
typedef enum {
  BLFM_SPI_MODE_0 = 0,  // CPOL=0, CPHA=0
  BLFM_SPI_MODE_1,      // CPOL=0, CPHA=1
  BLFM_SPI_MODE_2,      // CPOL=1, CPHA=0
  BLFM_SPI_MODE_3       // CPOL=1, CPHA=1
} blfm_spi_mode_t;

/**
 * @brief SPI data size enumeration
 */
typedef enum {
  BLFM_SPI_DATA_8BIT = 0,
  BLFM_SPI_DATA_16BIT
} blfm_spi_data_size_t;

/**
 * @brief SPI bit order enumeration
 */
typedef enum {
  BLFM_SPI_MSB_FIRST = 0,
  BLFM_SPI_LSB_FIRST
} blfm_spi_bit_order_t;

/**
 * @brief SPI configuration structure
 */
typedef struct {
  blfm_spi_mode_t mode;
  blfm_spi_data_size_t data_size;
  blfm_spi_bit_order_t bit_order;
  uint32_t prescaler;        // 2, 4, 8, 16, 32, 64, 128, 256
  uint32_t timeout_ms;
} blfm_spi_config_t;

/**
 * @brief SPI error codes
 */
typedef enum {
  BLFM_SPI_OK = 0,
  BLFM_SPI_ERR_INVALID_PERIPHERAL,
  BLFM_SPI_ERR_INVALID_CONFIG,
  BLFM_SPI_ERR_NOT_INITIALIZED,
  BLFM_SPI_ERR_TIMEOUT,
  BLFM_SPI_ERR_NULL_PTR,
  BLFM_SPI_ERR_BUSY
} blfm_spi_error_t;

/**
 * @brief Initialize SPI peripheral
 * @param peripheral SPI peripheral to initialize
 * @param config Configuration structure (can be NULL for default)
 * @return Error code
 */
blfm_spi_error_t blfm_spi_init(blfm_spi_peripheral_t peripheral, const blfm_spi_config_t *config);

/**
 * @brief Deinitialize SPI peripheral
 * @param peripheral SPI peripheral to deinitialize
 * @return Error code
 */
blfm_spi_error_t blfm_spi_deinit(blfm_spi_peripheral_t peripheral);

/**
 * @brief Transmit and receive data simultaneously
 * @param peripheral SPI peripheral
 * @param tx_data Transmit data buffer
 * @param rx_data Receive data buffer
 * @param length Number of bytes to transfer
 * @return Error code
 */
blfm_spi_error_t blfm_spi_transfer(blfm_spi_peripheral_t peripheral, 
                                   const uint8_t *tx_data, 
                                   uint8_t *rx_data, 
                                   size_t length);

/**
 * @brief Transmit data only
 * @param peripheral SPI peripheral
 * @param data Transmit data buffer
 * @param length Number of bytes to transmit
 * @return Error code
 */
blfm_spi_error_t blfm_spi_transmit(blfm_spi_peripheral_t peripheral, 
                                   const uint8_t *data, 
                                   size_t length);

/**
 * @brief Receive data only
 * @param peripheral SPI peripheral
 * @param data Receive data buffer
 * @param length Number of bytes to receive
 * @return Error code
 */
blfm_spi_error_t blfm_spi_receive(blfm_spi_peripheral_t peripheral, 
                                  uint8_t *data, 
                                  size_t length);

/**
 * @brief Send single byte and receive response
 * @param peripheral SPI peripheral
 * @param tx_byte Byte to transmit
 * @param rx_byte Pointer to store received byte
 * @return Error code
 */
blfm_spi_error_t blfm_spi_transfer_byte(blfm_spi_peripheral_t peripheral, 
                                        uint8_t tx_byte, 
                                        uint8_t *rx_byte);

/**
 * @brief Check if SPI peripheral is busy
 * @param peripheral SPI peripheral
 * @return true if busy, false if ready
 */
bool blfm_spi_is_busy(blfm_spi_peripheral_t peripheral);

#endif // BLFM_SPI_H