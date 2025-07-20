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
#include <stddef.h>
#include "stm32f1xx.h"

/**
 * SPI speed settings
 */
typedef enum {
    BLFM_SPI_SPEED_LOW,     // fPCLK/64 = 1.125MHz
    BLFM_SPI_SPEED_MEDIUM,  // fPCLK/16 = 4.5MHz
    BLFM_SPI_SPEED_HIGH     // fPCLK/4 = 18MHz
} blfm_spi_speed_t;

/**
 * Initialize SPI1 peripheral
 * 
 * @return 0 on success, -1 on error
 */
int blfm_spi1_init(void);

/**
 * Send and receive a byte via SPI1
 * 
 * @param data Byte to send
 * @return Received byte
 */
uint8_t blfm_spi1_transfer(uint8_t data);

/**
 * Write multiple bytes via SPI1
 * 
 * @param data Pointer to data buffer
 * @param len Number of bytes to write
 * @return 0 on success, -1 on error
 */
int blfm_spi1_write(const uint8_t *data, size_t len);

/**
 * Read multiple bytes via SPI1
 * 
 * @param data Pointer to receive buffer
 * @param len Number of bytes to read
 * @return 0 on success, -1 on error
 */
int blfm_spi1_read(uint8_t *data, size_t len);

/**
 * Write then read via SPI1 (typical for SPI devices)
 * 
 * @param tx_data Pointer to transmit data
 * @param tx_len Number of bytes to transmit
 * @param rx_data Pointer to receive buffer
 * @param rx_len Number of bytes to receive
 * @return 0 on success, -1 on error
 */
int blfm_spi1_write_read(const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);

/**
 * Control chip select (NSS) line
 */
void blfm_spi1_cs_low(void);
void blfm_spi1_cs_high(void);

/**
 * Set SPI communication speed
 * 
 * @param speed SPI speed setting
 */
void blfm_spi1_set_speed(blfm_spi_speed_t speed);

/**
 * Deinitialize SPI1 peripheral
 */
void blfm_spi1_deinit(void);

#endif /* BLFM_SPI_H */