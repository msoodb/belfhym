/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_spi.h"
#include "blfm_gpio.h"
#include "blfm_delay.h"
#include "blfm_pins.h"

// SPI Configuration
#define SPI_TIMEOUT_MS 100

// SPI state
static bool spi_initialized = false;

/**
 * Initialize SPI1 peripheral
 */
int blfm_spi1_init(void) {
    if (spi_initialized) {
        return 0; // Already initialized
    }
    
    // Enable SPI1 and GPIOA clocks
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    
    // Configure GPIO pins for SPI1
    // SCK (PA5) - Alternate function push-pull, 50MHz
    GPIOA->CRL &= ~(0xF << (5 * 4));
    GPIOA->CRL |= (0xB << (5 * 4));
    
    // MISO (PA6) - Input floating
    GPIOA->CRL &= ~(0xF << (6 * 4));
    GPIOA->CRL |= (0x4 << (6 * 4));
    
    // MOSI (PA7) - Alternate function push-pull, 50MHz
    GPIOA->CRL &= ~(0xF << (7 * 4));
    GPIOA->CRL |= (0xB << (7 * 4));
    
    // NSS (PA4) - General purpose output push-pull, 50MHz (software controlled)
    GPIOA->CRL &= ~(0xF << (4 * 4));
    GPIOA->CRL |= (0x3 << (4 * 4));
    GPIOA->BSRR = (1 << 4); // Set NSS high (inactive)
    
    // Configure SPI1
    SPI1->CR1 = 0; // Reset control register
    SPI1->CR1 |= SPI_CR1_MSTR;        // Master mode
    SPI1->CR1 |= SPI_CR1_BR_2;        // Baudrate = fPCLK/32 (72MHz/32 = 2.25MHz)
    SPI1->CR1 |= SPI_CR1_CPOL;        // Clock polarity high when idle
    SPI1->CR1 |= SPI_CR1_CPHA;        // Clock phase - data captured on rising edge
    SPI1->CR1 |= SPI_CR1_SSM;         // Software slave management
    SPI1->CR1 |= SPI_CR1_SSI;         // Internal slave select
    
    // Enable SPI1
    SPI1->CR1 |= SPI_CR1_SPE;
    
    spi_initialized = true;
    return 0;
}

/**
 * Send and receive a byte via SPI1
 */
uint8_t blfm_spi1_transfer(uint8_t data) {
    if (!spi_initialized) {
        return 0xFF;
    }
    
    // Wait for transmit buffer to be empty
    uint32_t timeout = SPI_TIMEOUT_MS * 1000;
    while (!(SPI1->SR & SPI_SR_TXE) && timeout--) {
        blfm_delay_us(1);
    }
    if (timeout == 0) return 0xFF;
    
    // Send data
    SPI1->DR = data;
    
    // Wait for receive buffer to have data
    timeout = SPI_TIMEOUT_MS * 1000;
    while (!(SPI1->SR & SPI_SR_RXNE) && timeout--) {
        blfm_delay_us(1);
    }
    if (timeout == 0) return 0xFF;
    
    // Return received data
    return (uint8_t)SPI1->DR;
}

/**
 * Write multiple bytes via SPI1
 */
int blfm_spi1_write(const uint8_t *data, size_t len) {
    if (!spi_initialized || !data || len == 0) {
        return -1;
    }
    
    for (size_t i = 0; i < len; i++) {
        blfm_spi1_transfer(data[i]);
    }
    
    return 0;
}

/**
 * Read multiple bytes via SPI1
 */
int blfm_spi1_read(uint8_t *data, size_t len) {
    if (!spi_initialized || !data || len == 0) {
        return -1;
    }
    
    for (size_t i = 0; i < len; i++) {
        data[i] = blfm_spi1_transfer(0xFF); // Send dummy byte to receive
    }
    
    return 0;
}

/**
 * Write then read via SPI1 (typical for SPI devices)
 */
int blfm_spi1_write_read(const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len) {
    if (!spi_initialized || !tx_data || !rx_data || tx_len == 0 || rx_len == 0) {
        return -1;
    }
    
    // Write phase
    for (size_t i = 0; i < tx_len; i++) {
        blfm_spi1_transfer(tx_data[i]);
    }
    
    // Read phase
    for (size_t i = 0; i < rx_len; i++) {
        rx_data[i] = blfm_spi1_transfer(0xFF);
    }
    
    return 0;
}

/**
 * Control chip select (NSS) line
 */
void blfm_spi1_cs_low(void) {
    GPIOA->BSRR = (1 << (4 + 16)); // Reset PA4 (NSS low - active)
}

void blfm_spi1_cs_high(void) {
    GPIOA->BSRR = (1 << 4); // Set PA4 (NSS high - inactive)
}

/**
 * Set SPI speed
 */
void blfm_spi1_set_speed(blfm_spi_speed_t speed) {
    if (!spi_initialized) return;
    
    // Disable SPI to change configuration
    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    // Clear existing baudrate bits
    SPI1->CR1 &= ~SPI_CR1_BR;
    
    // Set new baudrate
    switch (speed) {
        case BLFM_SPI_SPEED_HIGH:   // fPCLK/4 = 18MHz
            SPI1->CR1 |= SPI_CR1_BR_0;
            break;
        case BLFM_SPI_SPEED_MEDIUM: // fPCLK/16 = 4.5MHz  
            SPI1->CR1 |= SPI_CR1_BR_1;
            break;
        case BLFM_SPI_SPEED_LOW:    // fPCLK/64 = 1.125MHz
            SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
            break;
        default: // Default to medium speed
            SPI1->CR1 |= SPI_CR1_BR_1;
            break;
    }
    
    // Re-enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

/**
 * Deinitialize SPI1
 */
void blfm_spi1_deinit(void) {
    if (!spi_initialized) return;
    
    // Disable SPI1
    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    // Reset SPI1 configuration
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    
    // Disable SPI1 clock
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    
    spi_initialized = false;
}