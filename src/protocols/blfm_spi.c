/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_spi.h"
#include "blfm_pins.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"

#define SPI_DEFAULT_TIMEOUT_MS 100
#define SPI_DEFAULT_PRESCALER 64

/**
 * @brief SPI peripheral context structure
 */
typedef struct {
  SPI_TypeDef *reg;               // SPI register base address
  uint32_t rcc_apb_enr;           // RCC APB enable register
  uint32_t rcc_enable_mask;       // RCC enable mask for this peripheral
  GPIO_TypeDef *sck_port;         // SCK GPIO port
  uint8_t sck_pin;                // SCK GPIO pin
  GPIO_TypeDef *miso_port;        // MISO GPIO port
  uint8_t miso_pin;               // MISO GPIO pin
  GPIO_TypeDef *mosi_port;        // MOSI GPIO port
  uint8_t mosi_pin;               // MOSI GPIO pin
  uint32_t timeout_counter;       // Timeout counter value
  bool initialized;               // Initialization status
} blfm_spi_context_t;

// SPI peripheral contexts
static blfm_spi_context_t spi_contexts[BLFM_SPI_COUNT] = {
  [BLFM_SPI1] = {
    .reg = SPI1,
    .rcc_apb_enr = (uint32_t)&RCC->APB2ENR,
    .rcc_enable_mask = RCC_APB2ENR_SPI1EN,
    .sck_port = BLFM_SPI1_SCK_PORT,
    .sck_pin = BLFM_SPI1_SCK_PIN,
    .miso_port = BLFM_SPI1_MISO_PORT,
    .miso_pin = BLFM_SPI1_MISO_PIN,
    .mosi_port = BLFM_SPI1_MOSI_PORT,
    .mosi_pin = BLFM_SPI1_MOSI_PIN,
    .timeout_counter = 10000,
    .initialized = false
  },
  [BLFM_SPI2] = {
    .reg = SPI2,
    .rcc_apb_enr = (uint32_t)&RCC->APB1ENR,
    .rcc_enable_mask = RCC_APB1ENR_SPI2EN,
    .sck_port = BLFM_SPI2_SCK_PORT,
    .sck_pin = BLFM_SPI2_SCK_PIN,
    .miso_port = BLFM_SPI2_MISO_PORT,
    .miso_pin = BLFM_SPI2_MISO_PIN,
    .mosi_port = BLFM_SPI2_MOSI_PORT,
    .mosi_pin = BLFM_SPI2_MOSI_PIN,
    .timeout_counter = 10000,
    .initialized = false
  }
};

/**
 * @brief Wait for SPI flag with timeout
 * @param context SPI context
 * @param flag Flag to wait for
 * @param state Expected flag state
 * @return BLFM_SPI_OK on success, BLFM_SPI_ERR_TIMEOUT on timeout
 */
static blfm_spi_error_t blfm_spi_wait_flag(blfm_spi_context_t *context, uint32_t flag, bool state) {
  volatile uint32_t timeout = context->timeout_counter;
  while (((context->reg->SR & flag) != 0) != state) {
    if (--timeout == 0) {
      return BLFM_SPI_ERR_TIMEOUT;
    }
  }
  return BLFM_SPI_OK;
}

/**
 * @brief Configure GPIO pins for SPI
 * @param context SPI context
 */
static void blfm_spi_configure_gpio(blfm_spi_context_t *context) {
  // Enable GPIO clocks
  if (context->sck_port == GPIOA) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  } else if (context->sck_port == GPIOB) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  } else if (context->sck_port == GPIOC) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  }
  
  // Enable alternate function clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Configure SCK and MOSI as alternate function push-pull
  blfm_gpio_config_alternate_pushpull((uint32_t)context->sck_port, context->sck_pin);
  blfm_gpio_config_alternate_pushpull((uint32_t)context->mosi_port, context->mosi_pin);
  
  // Configure MISO as input floating
  blfm_gpio_config_input((uint32_t)context->miso_port, context->miso_pin);
}

/**
 * @brief Convert prescaler value to register bits
 * @param prescaler Prescaler value
 * @return Register bits
 */
static uint32_t blfm_spi_prescaler_to_bits(uint32_t prescaler) {
  switch (prescaler) {
    case 2:   return 0;
    case 4:   return SPI_CR1_BR_0;
    case 8:   return SPI_CR1_BR_1;
    case 16:  return SPI_CR1_BR_1 | SPI_CR1_BR_0;
    case 32:  return SPI_CR1_BR_2;
    case 64:  return SPI_CR1_BR_2 | SPI_CR1_BR_0;
    case 128: return SPI_CR1_BR_2 | SPI_CR1_BR_1;
    case 256: return SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
    default:  return SPI_CR1_BR_2 | SPI_CR1_BR_0; // Default to 64
  }
}

blfm_spi_error_t blfm_spi_init(blfm_spi_peripheral_t peripheral, const blfm_spi_config_t *config) {
  if (peripheral >= BLFM_SPI_COUNT) {
    return BLFM_SPI_ERR_INVALID_PERIPHERAL;
  }

  blfm_spi_context_t *ctx = &spi_contexts[peripheral];

  // Use default config if none provided
  blfm_spi_config_t default_config = {
    .mode = BLFM_SPI_MODE_0,
    .data_size = BLFM_SPI_DATA_8BIT,
    .bit_order = BLFM_SPI_MSB_FIRST,
    .prescaler = SPI_DEFAULT_PRESCALER,
    .timeout_ms = SPI_DEFAULT_TIMEOUT_MS
  };
  
  if (config == NULL) {
    config = &default_config;
  }

  // Update timeout counter based on config
  ctx->timeout_counter = config->timeout_ms * 100;

  // Enable SPI peripheral clock
  volatile uint32_t *rcc_reg = (volatile uint32_t *)ctx->rcc_apb_enr;
  *rcc_reg |= ctx->rcc_enable_mask;

  // Configure GPIO pins
  blfm_spi_configure_gpio(ctx);

  // Configure SPI
  uint32_t cr1 = 0;
  
  // Set mode (CPOL and CPHA)
  switch (config->mode) {
    case BLFM_SPI_MODE_0: /* CPOL=0, CPHA=0 */ break;
    case BLFM_SPI_MODE_1: cr1 |= SPI_CR1_CPHA; break;
    case BLFM_SPI_MODE_2: cr1 |= SPI_CR1_CPOL; break;
    case BLFM_SPI_MODE_3: cr1 |= SPI_CR1_CPOL | SPI_CR1_CPHA; break;
  }
  
  // Set data size
  if (config->data_size == BLFM_SPI_DATA_16BIT) {
    cr1 |= SPI_CR1_DFF;
  }
  
  // Set bit order
  if (config->bit_order == BLFM_SPI_LSB_FIRST) {
    cr1 |= SPI_CR1_LSBFIRST;
  }
  
  // Set prescaler
  cr1 |= blfm_spi_prescaler_to_bits(config->prescaler);
  
  // Master mode, software NSS
  cr1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

  // Apply configuration
  ctx->reg->CR1 = cr1;
  
  // Enable SPI
  ctx->reg->CR1 |= SPI_CR1_SPE;

  ctx->initialized = true;
  return BLFM_SPI_OK;
}

blfm_spi_error_t blfm_spi_deinit(blfm_spi_peripheral_t peripheral) {
  if (peripheral >= BLFM_SPI_COUNT) {
    return BLFM_SPI_ERR_INVALID_PERIPHERAL;
  }

  blfm_spi_context_t *ctx = &spi_contexts[peripheral];

  // Disable SPI peripheral
  ctx->reg->CR1 = 0;

  // Disable peripheral clock
  volatile uint32_t *rcc_reg = (volatile uint32_t *)ctx->rcc_apb_enr;
  *rcc_reg &= ~ctx->rcc_enable_mask;

  ctx->initialized = false;
  return BLFM_SPI_OK;
}

blfm_spi_error_t blfm_spi_transfer(blfm_spi_peripheral_t peripheral, 
                                   const uint8_t *tx_data, 
                                   uint8_t *rx_data, 
                                   size_t length) {
  if (peripheral >= BLFM_SPI_COUNT) {
    return BLFM_SPI_ERR_INVALID_PERIPHERAL;
  }
  if (length == 0) {
    return BLFM_SPI_ERR_INVALID_CONFIG;
  }

  blfm_spi_context_t *ctx = &spi_contexts[peripheral];
  if (!ctx->initialized) {
    return BLFM_SPI_ERR_NOT_INITIALIZED;
  }

  blfm_spi_error_t result;

  for (size_t i = 0; i < length; i++) {
    // Wait for TX buffer empty
    result = blfm_spi_wait_flag(ctx, SPI_SR_TXE, true);
    if (result != BLFM_SPI_OK) {
      return result;
    }

    // Send data
    ctx->reg->DR = tx_data ? tx_data[i] : 0x00;

    // Wait for RX buffer not empty
    result = blfm_spi_wait_flag(ctx, SPI_SR_RXNE, true);
    if (result != BLFM_SPI_OK) {
      return result;
    }

    // Read data
    uint8_t received = ctx->reg->DR;
    if (rx_data) {
      rx_data[i] = received;
    }
  }

  // Wait for SPI not busy
  result = blfm_spi_wait_flag(ctx, SPI_SR_BSY, false);
  return result;
}

blfm_spi_error_t blfm_spi_transmit(blfm_spi_peripheral_t peripheral, 
                                   const uint8_t *data, 
                                   size_t length) {
  if (data == NULL) {
    return BLFM_SPI_ERR_NULL_PTR;
  }
  return blfm_spi_transfer(peripheral, data, NULL, length);
}

blfm_spi_error_t blfm_spi_receive(blfm_spi_peripheral_t peripheral, 
                                  uint8_t *data, 
                                  size_t length) {
  if (data == NULL) {
    return BLFM_SPI_ERR_NULL_PTR;
  }
  return blfm_spi_transfer(peripheral, NULL, data, length);
}

blfm_spi_error_t blfm_spi_transfer_byte(blfm_spi_peripheral_t peripheral, 
                                        uint8_t tx_byte, 
                                        uint8_t *rx_byte) {
  return blfm_spi_transfer(peripheral, &tx_byte, rx_byte, 1);
}

bool blfm_spi_is_busy(blfm_spi_peripheral_t peripheral) {
  if (peripheral >= BLFM_SPI_COUNT) {
    return true;
  }

  blfm_spi_context_t *ctx = &spi_contexts[peripheral];
  if (!ctx->initialized) {
    return true;
  }

  return (ctx->reg->SR & SPI_SR_BSY) != 0;
}