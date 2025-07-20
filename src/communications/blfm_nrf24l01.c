/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_nrf24l01.h"
#include "blfm_config.h"

#if BLFM_ENABLED_NRF24L01

#include "blfm_spi.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "blfm_delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

// =============================================================================
// Private Variables
// =============================================================================

static bool nrf24_initialized = false;
static blfm_nrf24_config_t nrf24_config;
static blfm_nrf24_mode_t current_mode = NRF24_MODE_POWER_DOWN;

// =============================================================================
// Private Function Prototypes
// =============================================================================

static blfm_nrf24_result_t nrf24_spi_command(uint8_t command, uint8_t *data, uint8_t len);
static void nrf24_ce_high(void);
static void nrf24_ce_low(void);
static void nrf24_csn_high(void);
static void nrf24_csn_low(void);
static uint8_t nrf24_get_status(void);
static blfm_nrf24_result_t nrf24_wait_for_tx_done(uint32_t timeout_ms);
static void nrf24_apply_config(const blfm_nrf24_config_t *config);

// =============================================================================
// Private Functions
// =============================================================================

/**
 * @brief Execute SPI command with optional data
 */
static blfm_nrf24_result_t nrf24_spi_command(uint8_t command, uint8_t *data, uint8_t len) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }

    nrf24_csn_low();
    
    // Send command
    blfm_spi1_transfer(command);
    
    // Transfer data if provided
    if (data && len > 0) {
        for (uint8_t i = 0; i < len; i++) {
            data[i] = blfm_spi1_transfer(data[i]);
        }
    }
    
    nrf24_csn_high();
    
    return NRF24_SUCCESS;
}

/**
 * @brief Set CE pin high
 */
static void nrf24_ce_high(void) {
    blfm_gpio_set_pin((uint32_t)BLFM_NRF24_CE_PORT, BLFM_NRF24_CE_PIN);
}

/**
 * @brief Set CE pin low
 */
static void nrf24_ce_low(void) {
    blfm_gpio_clear_pin((uint32_t)BLFM_NRF24_CE_PORT, BLFM_NRF24_CE_PIN);
}

/**
 * @brief Set CSN pin high (deselect)
 */
static void nrf24_csn_high(void) {
    blfm_gpio_set_pin((uint32_t)BLFM_NRF24_CSN_PORT, BLFM_NRF24_CSN_PIN);
}

/**
 * @brief Set CSN pin low (select)
 */
static void nrf24_csn_low(void) {
    blfm_gpio_clear_pin((uint32_t)BLFM_NRF24_CSN_PORT, BLFM_NRF24_CSN_PIN);
}

/**
 * @brief Get status register value
 */
static uint8_t nrf24_get_status(void) {
    uint8_t status = 0;
    nrf24_spi_command(NRF24_CMD_NOP, &status, 0);
    return status;
}

/**
 * @brief Wait for TX operation to complete
 */
static blfm_nrf24_result_t nrf24_wait_for_tx_done(uint32_t timeout_ms) {
    TickType_t start_time = xTaskGetTickCount();
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    
    while ((xTaskGetTickCount() - start_time) < timeout_ticks) {
        uint8_t status = nrf24_get_status();
        
        if (status & NRF24_STATUS_TX_DS) {
            // Clear TX_DS flag
            blfm_nrf24_write_register(NRF24_REG_STATUS, NRF24_STATUS_TX_DS);
            return NRF24_SUCCESS;
        }
        
        if (status & NRF24_STATUS_MAX_RT) {
            // Max retransmits reached
            blfm_nrf24_write_register(NRF24_REG_STATUS, NRF24_STATUS_MAX_RT);
            blfm_nrf24_flush_tx();
            return NRF24_ERROR_TX_TIMEOUT;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    return NRF24_ERROR_TX_TIMEOUT;
}

/**
 * @brief Apply configuration to NRF24L01 registers
 */
static void nrf24_apply_config(const blfm_nrf24_config_t *config) {
    uint8_t reg_value;
    
    // Power down before configuration
    blfm_nrf24_power_down();
    
    // Set channel
    blfm_nrf24_write_register(NRF24_REG_RF_CH, config->channel & 0x7F);
    
    // Set RF setup (power and data rate)
    reg_value = (config->power << 1);
    switch (config->data_rate) {
        case NRF24_DATA_RATE_250K:
            reg_value |= NRF24_RF_SETUP_RF_DR_LOW;
            break;
        case NRF24_DATA_RATE_1M:
            // Default, no additional bits
            break;
        case NRF24_DATA_RATE_2M:
            reg_value |= NRF24_RF_SETUP_RF_DR_HIGH;
            break;
    }
    blfm_nrf24_write_register(NRF24_REG_RF_SETUP, reg_value);
    
    // Set auto retransmit
    reg_value = ((config->auto_retransmit_delay & 0x0F) << 4) | 
                (config->auto_retransmit_count & 0x0F);
    blfm_nrf24_write_register(NRF24_REG_SETUP_RETR, reg_value);
    
    // Set address width (5 bytes)
    blfm_nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);
    
    // Configure pipes
    uint8_t en_aa = 0;
    uint8_t en_rxaddr = 0;
    
    for (int i = 0; i < 6; i++) {
        if (config->pipes[i].enabled) {
            en_rxaddr |= (1 << i);
            
            if (config->pipes[i].auto_ack) {
                en_aa |= (1 << i);
            }
            
            // Set payload size
            blfm_nrf24_write_register(NRF24_REG_RX_PW_P0 + i, 
                                    config->pipes[i].payload_size & 0x3F);
            
            // Set RX address
            if (i <= 1) {
                // Pipes 0 and 1 have full 5-byte addresses
                blfm_nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0 + i,
                                               config->pipes[i].address, 
                                               NRF24_ADDR_WIDTH);
            } else {
                // Pipes 2-5 only have LSB different from pipe 1
                blfm_nrf24_write_register(NRF24_REG_RX_ADDR_P0 + i,
                                        config->pipes[i].address[0]);
            }
        }
    }
    
    blfm_nrf24_write_register(NRF24_REG_EN_AA, en_aa);
    blfm_nrf24_write_register(NRF24_REG_EN_RXADDR, en_rxaddr);
    
    // Set TX address
    blfm_nrf24_write_register_multi(NRF24_REG_TX_ADDR, 
                                   config->tx_address, 
                                   NRF24_ADDR_WIDTH);
    
    // Set config register (CRC, power up in standby mode)
    reg_value = NRF24_CONFIG_PWR_UP;
    switch (config->crc) {
        case NRF24_CRC_DISABLED:
            // No CRC
            break;
        case NRF24_CRC_1_BYTE:
            reg_value |= NRF24_CONFIG_EN_CRC;
            break;
        case NRF24_CRC_2_BYTE:
            reg_value |= NRF24_CONFIG_EN_CRC | NRF24_CONFIG_CRCO;
            break;
    }
    blfm_nrf24_write_register(NRF24_REG_CONFIG, reg_value);
    
    // Wait for power up
    vTaskDelay(pdMS_TO_TICKS(5));
    
    current_mode = NRF24_MODE_STANDBY;
}

// =============================================================================
// Public Functions
// =============================================================================

blfm_nrf24_result_t blfm_nrf24_init(const blfm_nrf24_config_t *config) {
    if (!config) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    // Initialize SPI
    if (blfm_spi1_init() != 0) {
        return NRF24_ERROR_HARDWARE;
    }
    
    // Configure GPIO pins
    blfm_gpio_config_output((uint32_t)BLFM_NRF24_CE_PORT, BLFM_NRF24_CE_PIN);
    blfm_gpio_config_output((uint32_t)BLFM_NRF24_CSN_PORT, BLFM_NRF24_CSN_PIN);
    
    // Initialize pins to safe state
    nrf24_ce_low();
    nrf24_csn_high();
    
    // Set SPI speed for NRF24L01 (max 10MHz)
    blfm_spi1_set_speed(BLFM_SPI_SPEED_MEDIUM);  // 4.5MHz
    
    // Small delay for stabilization
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Test connection
    if (!blfm_nrf24_test_connection()) {
        return NRF24_ERROR_HARDWARE;
    }
    
    // Copy configuration
    memcpy(&nrf24_config, config, sizeof(blfm_nrf24_config_t));
    
    // Apply configuration
    nrf24_apply_config(config);
    
    // Clear any pending interrupts
    blfm_nrf24_write_register(NRF24_REG_STATUS, 
                             NRF24_STATUS_RX_DR | 
                             NRF24_STATUS_TX_DS | 
                             NRF24_STATUS_MAX_RT);
    
    // Flush FIFOs
    blfm_nrf24_flush_tx();
    blfm_nrf24_flush_rx();
    
    nrf24_initialized = true;
    
    return NRF24_SUCCESS;
}

void blfm_nrf24_deinit(void) {
    if (!nrf24_initialized) {
        return;
    }
    
    blfm_nrf24_power_down();
    nrf24_ce_low();
    nrf24_csn_high();
    
    nrf24_initialized = false;
}

blfm_nrf24_result_t blfm_nrf24_set_mode(blfm_nrf24_mode_t mode) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    uint8_t config_reg = blfm_nrf24_read_register(NRF24_REG_CONFIG);
    
    switch (mode) {
        case NRF24_MODE_POWER_DOWN:
            config_reg &= ~NRF24_CONFIG_PWR_UP;
            blfm_nrf24_write_register(NRF24_REG_CONFIG, config_reg);
            nrf24_ce_low();
            current_mode = NRF24_MODE_POWER_DOWN;
            break;
            
        case NRF24_MODE_STANDBY:
            config_reg |= NRF24_CONFIG_PWR_UP;
            config_reg &= ~NRF24_CONFIG_PRIM_RX;
            blfm_nrf24_write_register(NRF24_REG_CONFIG, config_reg);
            nrf24_ce_low();
            vTaskDelay(pdMS_TO_TICKS(2));
            current_mode = NRF24_MODE_STANDBY;
            break;
            
        case NRF24_MODE_TX:
            config_reg |= NRF24_CONFIG_PWR_UP;
            config_reg &= ~NRF24_CONFIG_PRIM_RX;
            blfm_nrf24_write_register(NRF24_REG_CONFIG, config_reg);
            vTaskDelay(pdMS_TO_TICKS(2));
            current_mode = NRF24_MODE_TX;
            break;
            
        case NRF24_MODE_RX:
            config_reg |= NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_RX;
            blfm_nrf24_write_register(NRF24_REG_CONFIG, config_reg);
            nrf24_ce_high();
            vTaskDelay(pdMS_TO_TICKS(2));
            current_mode = NRF24_MODE_RX;
            break;
            
        default:
            return NRF24_ERROR_INVALID_PARAM;
    }
    
    return NRF24_SUCCESS;
}

blfm_nrf24_mode_t blfm_nrf24_get_mode(void) {
    return current_mode;
}

blfm_nrf24_result_t blfm_nrf24_set_channel(uint8_t channel) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    if (channel > NRF24_MAX_CHANNEL) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    blfm_nrf24_write_register(NRF24_REG_RF_CH, channel);
    nrf24_config.channel = channel;
    
    return NRF24_SUCCESS;
}

uint8_t blfm_nrf24_get_channel(void) {
    if (!nrf24_initialized) {
        return 0;
    }
    
    return blfm_nrf24_read_register(NRF24_REG_RF_CH) & 0x7F;
}

blfm_nrf24_result_t blfm_nrf24_set_tx_address(const uint8_t *address) {
    if (!nrf24_initialized || !address) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    blfm_nrf24_result_t result = blfm_nrf24_write_register_multi(NRF24_REG_TX_ADDR, 
                                                               address, 
                                                               NRF24_ADDR_WIDTH);
    
    if (result == NRF24_SUCCESS) {
        memcpy(nrf24_config.tx_address, address, NRF24_ADDR_WIDTH);
        
        // Also set pipe 0 RX address for auto-ack
        blfm_nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0, 
                                       address, 
                                       NRF24_ADDR_WIDTH);
    }
    
    return result;
}

blfm_nrf24_result_t blfm_nrf24_set_rx_address(uint8_t pipe, const uint8_t *address) {
    if (!nrf24_initialized || !address || pipe > 5) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    blfm_nrf24_result_t result;
    
    if (pipe <= 1) {
        // Pipes 0 and 1 have full 5-byte addresses
        result = blfm_nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0 + pipe,
                                               address, 
                                               NRF24_ADDR_WIDTH);
        if (result == NRF24_SUCCESS) {
            memcpy(nrf24_config.pipes[pipe].address, address, NRF24_ADDR_WIDTH);
        }
    } else {
        // Pipes 2-5 only have LSB different from pipe 1
        result = blfm_nrf24_write_register(NRF24_REG_RX_ADDR_P0 + pipe, address[0]);
        if (result == NRF24_SUCCESS) {
            nrf24_config.pipes[pipe].address[0] = address[0];
        }
    }
    
    return result;
}

blfm_nrf24_result_t blfm_nrf24_enable_pipe(uint8_t pipe, bool enable) {
    if (!nrf24_initialized || pipe > 5) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    uint8_t en_rxaddr = blfm_nrf24_read_register(NRF24_REG_EN_RXADDR);
    
    if (enable) {
        en_rxaddr |= (1 << pipe);
    } else {
        en_rxaddr &= ~(1 << pipe);
    }
    
    blfm_nrf24_write_register(NRF24_REG_EN_RXADDR, en_rxaddr);
    nrf24_config.pipes[pipe].enabled = enable;
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_set_payload_size(uint8_t pipe, uint8_t size) {
    if (!nrf24_initialized || pipe > 5 || size == 0 || size > NRF24_MAX_PAYLOAD_SIZE) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    blfm_nrf24_write_register(NRF24_REG_RX_PW_P0 + pipe, size & 0x3F);
    nrf24_config.pipes[pipe].payload_size = size;
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_send(const uint8_t *data, uint8_t size, uint32_t timeout_ms) {
    if (!nrf24_initialized || !data || size == 0 || size > NRF24_MAX_PAYLOAD_SIZE) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    // Switch to TX mode
    blfm_nrf24_result_t result = blfm_nrf24_set_mode(NRF24_MODE_TX);
    if (result != NRF24_SUCCESS) {
        return result;
    }
    
    // Clear any pending TX flags
    blfm_nrf24_write_register(NRF24_REG_STATUS, 
                             NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
    
    // Write payload to TX FIFO
    uint8_t tx_data[NRF24_MAX_PAYLOAD_SIZE];
    memcpy(tx_data, data, size);
    
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < size; i++) {
        blfm_spi1_transfer(tx_data[i]);
    }
    nrf24_csn_high();
    
    // Pulse CE to start transmission
    nrf24_ce_high();
    blfm_delay_us(NRF24_CE_PULSE_WIDTH_US);
    nrf24_ce_low();
    
    // Wait for transmission to complete
    result = nrf24_wait_for_tx_done(timeout_ms);
    
    return result;
}

blfm_nrf24_result_t blfm_nrf24_receive(blfm_nrf24_packet_t *packet) {
    if (!nrf24_initialized || !packet) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    // Check if data is available
    if (!blfm_nrf24_data_available()) {
        return NRF24_ERROR_RX_TIMEOUT;
    }
    
    uint8_t status = nrf24_get_status();
    uint8_t pipe_number = (status & NRF24_STATUS_RX_P_NO) >> 1;
    
    if (pipe_number >= 6) {
        return NRF24_ERROR_HARDWARE;
    }
    
    // Get payload size
    uint8_t payload_size;
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_R_RX_PL_WID);
    payload_size = blfm_spi1_transfer(NRF24_CMD_NOP);
    nrf24_csn_high();
    
    if (payload_size > NRF24_MAX_PAYLOAD_SIZE) {
        // Invalid payload size, flush RX FIFO
        blfm_nrf24_flush_rx();
        return NRF24_ERROR_HARDWARE;
    }
    
    // Read payload
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_R_RX_PAYLOAD);
    for (uint8_t i = 0; i < payload_size; i++) {
        packet->data[i] = blfm_spi1_transfer(NRF24_CMD_NOP);
    }
    nrf24_csn_high();
    
    packet->size = payload_size;
    packet->pipe = pipe_number;
    packet->timestamp = xTaskGetTickCount();
    
    // Clear RX_DR flag
    blfm_nrf24_write_register(NRF24_REG_STATUS, NRF24_STATUS_RX_DR);
    
    return NRF24_SUCCESS;
}

bool blfm_nrf24_data_available(void) {
    if (!nrf24_initialized) {
        return false;
    }
    
    uint8_t status = nrf24_get_status();
    return (status & NRF24_STATUS_RX_DR) != 0;
}

bool blfm_nrf24_data_available_pipe(uint8_t pipe) {
    if (!nrf24_initialized || pipe > 5) {
        return false;
    }
    
    if (!blfm_nrf24_data_available()) {
        return false;
    }
    
    uint8_t status = nrf24_get_status();
    uint8_t rx_pipe = (status & NRF24_STATUS_RX_P_NO) >> 1;
    
    return rx_pipe == pipe;
}

blfm_nrf24_result_t blfm_nrf24_get_status(blfm_nrf24_status_t *status) {
    if (!nrf24_initialized || !status) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    uint8_t status_reg = nrf24_get_status();
    uint8_t fifo_status = blfm_nrf24_read_register(NRF24_REG_FIFO_STATUS);
    uint8_t observe_tx = blfm_nrf24_read_register(NRF24_REG_OBSERVE_TX);
    
    status->data_ready = (status_reg & NRF24_STATUS_RX_DR) != 0;
    status->data_sent = (status_reg & NRF24_STATUS_TX_DS) != 0;
    status->max_retransmit = (status_reg & NRF24_STATUS_MAX_RT) != 0;
    status->rx_pipe_number = (status_reg & NRF24_STATUS_RX_P_NO) >> 1;
    status->tx_fifo_full = (status_reg & NRF24_STATUS_TX_FULL) != 0;
    
    status->rx_fifo_full = (fifo_status & NRF24_FIFO_STATUS_RX_FULL) != 0;
    status->rx_fifo_empty = (fifo_status & NRF24_FIFO_STATUS_RX_EMPTY) != 0;
    status->tx_fifo_empty = (fifo_status & NRF24_FIFO_STATUS_TX_EMPTY) != 0;
    
    status->lost_packets = (observe_tx & 0xF0) >> 4;
    status->retransmit_count = observe_tx & 0x0F;
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_clear_status(uint8_t flags) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    blfm_nrf24_write_register(NRF24_REG_STATUS, flags);
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_flush_tx(void) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    return nrf24_spi_command(NRF24_CMD_FLUSH_TX, NULL, 0);
}

blfm_nrf24_result_t blfm_nrf24_flush_rx(void) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    return nrf24_spi_command(NRF24_CMD_FLUSH_RX, NULL, 0);
}

blfm_nrf24_result_t blfm_nrf24_power_up(void) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    uint8_t config = blfm_nrf24_read_register(NRF24_REG_CONFIG);
    config |= NRF24_CONFIG_PWR_UP;
    blfm_nrf24_write_register(NRF24_REG_CONFIG, config);
    
    vTaskDelay(pdMS_TO_TICKS(5));
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_power_down(void) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    uint8_t config = blfm_nrf24_read_register(NRF24_REG_CONFIG);
    config &= ~NRF24_CONFIG_PWR_UP;
    blfm_nrf24_write_register(NRF24_REG_CONFIG, config);
    
    nrf24_ce_low();
    current_mode = NRF24_MODE_POWER_DOWN;
    
    return NRF24_SUCCESS;
}

bool blfm_nrf24_test_connection(void) {
    // Test by writing and reading setup register
    uint8_t test_value = 0x55;
    blfm_nrf24_write_register(NRF24_REG_SETUP_AW, test_value);
    
    uint8_t read_value = blfm_nrf24_read_register(NRF24_REG_SETUP_AW);
    
    // Restore default value
    blfm_nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);
    
    return (read_value == test_value);
}

void blfm_nrf24_get_default_config(blfm_nrf24_config_t *config) {
    if (!config) {
        return;
    }
    
    memset(config, 0, sizeof(blfm_nrf24_config_t));
    
    config->channel = NRF24_DEFAULT_CHANNEL;
    config->data_rate = NRF24_DATA_RATE_1M;
    config->power = NRF24_POWER_0_DBM;
    config->crc = NRF24_CRC_2_BYTE;
    config->auto_retransmit_count = 3;
    config->auto_retransmit_delay = 5;
    
    // Default TX address
    uint8_t default_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    memcpy(config->tx_address, default_addr, NRF24_ADDR_WIDTH);
    
    // Configure pipe 0 for auto-ack
    config->pipes[0].enabled = true;
    config->pipes[0].auto_ack = true;
    config->pipes[0].payload_size = 32;
    memcpy(config->pipes[0].address, default_addr, NRF24_ADDR_WIDTH);
    
    // Configure pipe 1 for reception
    uint8_t rx_addr[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
    config->pipes[1].enabled = true;
    config->pipes[1].auto_ack = true;
    config->pipes[1].payload_size = 32;
    memcpy(config->pipes[1].address, rx_addr, NRF24_ADDR_WIDTH);
}

// =============================================================================
// Low-level Register Access
// =============================================================================

uint8_t blfm_nrf24_read_register(uint8_t reg) {
    if (!nrf24_initialized) {
        return 0;
    }
    
    uint8_t value = 0;
    
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    value = blfm_spi1_transfer(NRF24_CMD_NOP);
    nrf24_csn_high();
    
    return value;
}

blfm_nrf24_result_t blfm_nrf24_write_register(uint8_t reg, uint8_t value) {
    if (!nrf24_initialized) {
        return NRF24_ERROR_NOT_INITIALIZED;
    }
    
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    blfm_spi1_transfer(value);
    nrf24_csn_high();
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_read_register_multi(uint8_t reg, uint8_t *data, uint8_t len) {
    if (!nrf24_initialized || !data || len == 0) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    for (uint8_t i = 0; i < len; i++) {
        data[i] = blfm_spi1_transfer(NRF24_CMD_NOP);
    }
    nrf24_csn_high();
    
    return NRF24_SUCCESS;
}

blfm_nrf24_result_t blfm_nrf24_write_register_multi(uint8_t reg, const uint8_t *data, uint8_t len) {
    if (!nrf24_initialized || !data || len == 0) {
        return NRF24_ERROR_INVALID_PARAM;
    }
    
    nrf24_csn_low();
    blfm_spi1_transfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    for (uint8_t i = 0; i < len; i++) {
        blfm_spi1_transfer(data[i]);
    }
    nrf24_csn_high();
    
    return NRF24_SUCCESS;
}

#endif /* BLFM_ENABLED_NRF24L01 */