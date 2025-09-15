/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Homa.
 *
 * Homa is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_UART_H
#define BLFM_UART_H

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/*                          PUBLIC CONSTANTS                                 */
/* ========================================================================== */

#define UART_MAX_INSTANCES              3
#define UART_RX_BUFFER_SIZE             256
#define UART_PRINTF_BUFFER_SIZE         128
#define UART_TIMEOUT_VALUE              100000
#define UART_INTERRUPT_PRIORITY         5

/* ========================================================================== */
/*                          PUBLIC ENUMERATIONS                              */
/* ========================================================================== */

/**
 * @brief UART instances
 */
typedef enum {
    BLFM_UART1 = 0,
    BLFM_UART2 = 1,
    BLFM_UART3 = 2
} blfm_uart_instance_t;

/**
 * @brief UART status codes
 */
typedef enum {
    BLFM_UART_OK = 0,
    BLFM_UART_ERROR_INVALID_PARAM,
    BLFM_UART_ERROR_NOT_INITIALIZED,
    BLFM_UART_ERROR_ALREADY_INITIALIZED,
    BLFM_UART_ERROR_TIMEOUT,
    BLFM_UART_ERROR_NO_DATA,
    BLFM_UART_ERROR_BUSY
} blfm_uart_status_t;

/**
 * @brief UART word length
 */
typedef enum {
    BLFM_UART_WORDLENGTH_8B = 0,
    BLFM_UART_WORDLENGTH_9B = 1
} blfm_uart_word_length_t;

/**
 * @brief UART stop bits
 */
typedef enum {
    BLFM_UART_STOPBITS_1 = 0,
    BLFM_UART_STOPBITS_0_5 = 1,
    BLFM_UART_STOPBITS_2 = 2,
    BLFM_UART_STOPBITS_1_5 = 3
} blfm_uart_stop_bits_t;

/**
 * @brief UART parity
 */
typedef enum {
    BLFM_UART_PARITY_NONE = 0,
    BLFM_UART_PARITY_EVEN = 1,
    BLFM_UART_PARITY_ODD = 2
} blfm_uart_parity_t;

/**
 * @brief UART hardware flow control
 */
typedef enum {
    BLFM_UART_HWCONTROL_NONE = 0,
    BLFM_UART_HWCONTROL_RTS = 1,
    BLFM_UART_HWCONTROL_CTS = 2,
    BLFM_UART_HWCONTROL_RTS_CTS = 3
} blfm_uart_hw_flow_control_t;

/**
 * @brief UART operating mode
 */
typedef enum {
    BLFM_UART_MODE_POLLING = 0,
    BLFM_UART_MODE_INTERRUPT = 1,
    BLFM_UART_MODE_DMA = 2
} blfm_uart_mode_t;

/* ========================================================================== */
/*                          PUBLIC STRUCTURES                                */
/* ========================================================================== */

/**
 * @brief UART configuration structure
 */
typedef struct {
    uint32_t baudrate;                          /* Baud rate (9600, 115200, etc.) */
    blfm_uart_word_length_t word_length;        /* Data bits */
    blfm_uart_stop_bits_t stop_bits;            /* Stop bits */
    blfm_uart_parity_t parity;                  /* Parity */
    blfm_uart_hw_flow_control_t hw_flow_control; /* Hardware flow control */
    blfm_uart_mode_t mode;                      /* Operating mode */
} blfm_uart_config_t;

/* ========================================================================== */
/*                          PUBLIC FUNCTION PROTOTYPES                       */
/* ========================================================================== */

/* ========================================================================== */
/*                          DRIVER LAYER FUNCTIONS                           */
/* ========================================================================== */

/**
 * @brief Initialize UART instance
 * @param instance UART instance to initialize
 * @param config Configuration parameters
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_init(blfm_uart_instance_t instance, const blfm_uart_config_t *config);

/**
 * @brief Deinitialize UART instance
 * @param instance UART instance to deinitialize
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_deinit(blfm_uart_instance_t instance);

/**
 * @brief Transmit single byte (blocking)
 * @param instance UART instance
 * @param data Byte to transmit
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_transmit_byte(blfm_uart_instance_t instance, uint8_t data);

/**
 * @brief Receive single byte (blocking with timeout)
 * @param instance UART instance
 * @param data Pointer to store received byte
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_receive_byte(blfm_uart_instance_t instance, uint8_t *data);

/**
 * @brief Transmit buffer (blocking)
 * @param instance UART instance
 * @param data Buffer to transmit
 * @param size Number of bytes to transmit
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_transmit(blfm_uart_instance_t instance, const uint8_t *data, uint16_t size);

/**
 * @brief Receive buffer (blocking with timeout)
 * @param instance UART instance
 * @param data Buffer to store received data
 * @param size Number of bytes to receive
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_receive(blfm_uart_instance_t instance, uint8_t *data, uint16_t size);

/* ========================================================================== */
/*                          PROTOCOL LAYER FUNCTIONS                         */
/* ========================================================================== */

/**
 * @brief Transmit null-terminated string
 * @param instance UART instance
 * @param str String to transmit
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_transmit_string(blfm_uart_instance_t instance, const char *str);

/**
 * @brief Receive string until delimiter or buffer full
 * @param instance UART instance
 * @param buffer Buffer to store string
 * @param buffer_size Size of buffer
 * @param delimiter Character to stop reception
 * @return BLFM_UART_OK on success, error code otherwise
 */
blfm_uart_status_t blfm_uart_receive_string(blfm_uart_instance_t instance, char *buffer, uint16_t buffer_size, char delimiter);

/**
 * @brief Printf implementation via UART
 * @param format Printf format string
 * @param ... Printf arguments
 * @return Number of characters written
 */
int blfm_uart_printf(const char *format, ...);

/**
 * @brief Set UART instance for printf output
 * @param instance UART instance to use for printf
 */
void blfm_uart_set_printf_uart(blfm_uart_instance_t instance);

/* ========================================================================== */
/*                          INTERRUPT/BUFFER FUNCTIONS                       */
/* ========================================================================== */

/**
 * @brief Get number of bytes available in RX buffer
 * @param instance UART instance
 * @return Number of bytes available
 */
uint16_t blfm_uart_available(blfm_uart_instance_t instance);

/**
 * @brief Read byte from RX buffer (non-blocking)
 * @param instance UART instance
 * @param data Pointer to store received byte
 * @return BLFM_UART_OK on success, BLFM_UART_ERROR_NO_DATA if buffer empty
 */
blfm_uart_status_t blfm_uart_read_buffer(blfm_uart_instance_t instance, uint8_t *data);

/* ========================================================================== */
/*                          CONVENIENCE MACROS                               */
/* ========================================================================== */

/**
 * @brief Default UART configuration for debug console
 */
#define BLFM_UART_CONFIG_DEFAULT() { \
    .baudrate = 115200, \
    .word_length = BLFM_UART_WORDLENGTH_8B, \
    .stop_bits = BLFM_UART_STOPBITS_1, \
    .parity = BLFM_UART_PARITY_NONE, \
    .hw_flow_control = BLFM_UART_HWCONTROL_NONE, \
    .mode = BLFM_UART_MODE_POLLING \
}

/**
 * @brief UART configuration with interrupt mode
 */
#define BLFM_UART_CONFIG_INTERRUPT() { \
    .baudrate = 115200, \
    .word_length = BLFM_UART_WORDLENGTH_8B, \
    .stop_bits = BLFM_UART_STOPBITS_1, \
    .parity = BLFM_UART_PARITY_NONE, \
    .hw_flow_control = BLFM_UART_HWCONTROL_NONE, \
    .mode = BLFM_UART_MODE_INTERRUPT \
}

#endif /* BLFM_UART_H */