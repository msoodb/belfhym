/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_i2c.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"

#define I2C_DEFAULT_TIMEOUT_MS 100
#define I2C_DEFAULT_SPEED_HZ 100000  // 100 kHz
#define I2C_APB1_FREQ_MHZ 36         // APB1 clock frequency

/**
 * @brief I2C peripheral context structure
 */
typedef struct {
  I2C_TypeDef *reg;           // I2C register base address
  uint32_t rcc_enable_mask;   // RCC enable mask for this peripheral
  GPIO_TypeDef *scl_port;     // SCL GPIO port
  uint8_t scl_pin;            // SCL GPIO pin
  GPIO_TypeDef *sda_port;     // SDA GPIO port
  uint8_t sda_pin;            // SDA GPIO pin
  uint32_t timeout_counter;   // Timeout counter value
  bool initialized;           // Initialization status
} blfm_i2c_context_t;

// I2C peripheral contexts
static blfm_i2c_context_t i2c_contexts[BLFM_I2C_COUNT] = {
  [BLFM_I2C1] = {
    .reg = I2C1,
    .rcc_enable_mask = RCC_APB1ENR_I2C1EN,
    .scl_port = BLFM_I2C1_SCL_PORT,
    .scl_pin = BLFM_I2C1_SCL_PIN,
    .sda_port = BLFM_I2C1_SDA_PORT,
    .sda_pin = BLFM_I2C1_SDA_PIN,
    .timeout_counter = 10000,
    .initialized = false
  },
  [BLFM_I2C2] = {
    .reg = I2C2,
    .rcc_enable_mask = RCC_APB1ENR_I2C2EN,
    .scl_port = BLFM_I2C2_SCL_PORT,
    .scl_pin = BLFM_I2C2_SCL_PIN,
    .sda_port = BLFM_I2C2_SDA_PORT,
    .sda_pin = BLFM_I2C2_SDA_PIN,
    .timeout_counter = 10000,
    .initialized = false
  }
};

/**
 * @brief Wait for I2C event with timeout
 * @param context I2C context
 * @param flag Event flag to wait for
 * @return BLFM_I2C_OK on success, BLFM_I2C_ERR_TIMEOUT on timeout
 */
static blfm_i2c_error_t blfm_i2c_wait_event(blfm_i2c_context_t *context, uint32_t flag) {
  volatile uint32_t timeout = context->timeout_counter;
  while (!(context->reg->SR1 & flag)) {
    if (--timeout == 0) {
      return BLFM_I2C_ERR_TIMEOUT;
    }
  }
  return BLFM_I2C_OK;
}

/**
 * @brief Configure GPIO pins for I2C
 * @param context I2C context
 */
static void blfm_i2c_configure_gpio(blfm_i2c_context_t *context) {
  // Enable GPIO clocks
  if (context->scl_port == GPIOA) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  } else if (context->scl_port == GPIOB) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  } else if (context->scl_port == GPIOC) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  }
  
  // Enable alternate function clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Configure SCL and SDA pins as alternate function open-drain
  uint32_t scl_pos = context->scl_pin * 4;
  uint32_t sda_pos = context->sda_pin * 4;

  if (context->scl_pin < 8) {
    context->scl_port->CRL &= ~(0xF << scl_pos);
    context->scl_port->CRL |= (0xB << scl_pos); // Alternate Open-Drain, 50MHz
  } else {
    scl_pos = (context->scl_pin - 8) * 4;
    context->scl_port->CRH &= ~(0xF << scl_pos);
    context->scl_port->CRH |= (0xB << scl_pos);
  }

  if (context->sda_pin < 8) {
    context->sda_port->CRL &= ~(0xF << sda_pos);
    context->sda_port->CRL |= (0xB << sda_pos); // Alternate Open-Drain, 50MHz
  } else {
    sda_pos = (context->sda_pin - 8) * 4;
    context->sda_port->CRH &= ~(0xF << sda_pos);
    context->sda_port->CRH |= (0xB << sda_pos);
  }
}

blfm_i2c_error_t blfm_i2c_init(blfm_i2c_peripheral_t peripheral, const blfm_i2c_config_t *config) {
  if (peripheral >= BLFM_I2C_COUNT) {
    return BLFM_I2C_ERR_INVALID_PERIPHERAL;
  }

  blfm_i2c_context_t *ctx = &i2c_contexts[peripheral];

  // Use default config if none provided
  blfm_i2c_config_t default_config = {
    .speed_hz = I2C_DEFAULT_SPEED_HZ,
    .timeout_ms = I2C_DEFAULT_TIMEOUT_MS
  };
  
  if (config == NULL) {
    config = &default_config;
  }

  // Update timeout counter based on config
  ctx->timeout_counter = config->timeout_ms * 100; // Rough approximation

  // Enable I2C peripheral clock
  RCC->APB1ENR |= ctx->rcc_enable_mask;

  // Configure GPIO pins
  blfm_i2c_configure_gpio(ctx);

  // Reset I2C peripheral
  ctx->reg->CR1 = I2C_CR1_SWRST;
  ctx->reg->CR1 = 0;

  // Configure I2C
  ctx->reg->CR2 = I2C_APB1_FREQ_MHZ;

  // Calculate CCR value based on desired speed
  uint32_t ccr_value;
  if (config->speed_hz <= 100000) {
    // Standard mode (up to 100 kHz)
    ccr_value = (I2C_APB1_FREQ_MHZ * 1000000) / (2 * config->speed_hz);
    ctx->reg->CCR = ccr_value;
    ctx->reg->TRISE = I2C_APB1_FREQ_MHZ + 1;
  } else {
    // Fast mode (up to 400 kHz)
    ccr_value = (I2C_APB1_FREQ_MHZ * 1000000) / (3 * config->speed_hz);
    ctx->reg->CCR = ccr_value | I2C_CCR_FS;
    ctx->reg->TRISE = ((I2C_APB1_FREQ_MHZ * 300) / 1000) + 1;
  }

  // Enable I2C peripheral
  ctx->reg->CR1 |= I2C_CR1_PE;

  ctx->initialized = true;
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_deinit(blfm_i2c_peripheral_t peripheral) {
  if (peripheral >= BLFM_I2C_COUNT) {
    return BLFM_I2C_ERR_INVALID_PERIPHERAL;
  }

  blfm_i2c_context_t *ctx = &i2c_contexts[peripheral];

  // Disable I2C peripheral
  ctx->reg->CR1 = 0;

  // Disable peripheral clock
  RCC->APB1ENR &= ~ctx->rcc_enable_mask;

  ctx->initialized = false;
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_write(blfm_i2c_peripheral_t peripheral, uint8_t addr, const uint8_t *data, size_t len) {
  if (peripheral >= BLFM_I2C_COUNT) {
    return BLFM_I2C_ERR_INVALID_PERIPHERAL;
  }
  if (data == NULL || len == 0) {
    return BLFM_I2C_ERR_NULL_PTR;
  }

  blfm_i2c_context_t *ctx = &i2c_contexts[peripheral];
  if (!ctx->initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  ctx->reg->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(ctx, I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (write)
  (void)ctx->reg->SR1; // Clear SB flag
  ctx->reg->DR = addr << 1; // Write mode
  result = blfm_i2c_wait_event(ctx, I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Clear ADDR flag
  (void)ctx->reg->SR1;
  (void)ctx->reg->SR2;

  // Send data bytes
  for (size_t i = 0; i < len; i++) {
    ctx->reg->DR = data[i];
    result = blfm_i2c_wait_event(ctx, I2C_SR1_TXE);
    if (result != BLFM_I2C_OK) {
      return result;
    }
  }

  // Wait for transfer complete
  while (!(ctx->reg->SR1 & I2C_SR1_BTF)) {
    // Wait for byte transfer finished
  }

  // Generate STOP condition
  ctx->reg->CR1 |= I2C_CR1_STOP;

  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_read(blfm_i2c_peripheral_t peripheral, uint8_t addr, uint8_t *data, size_t len) {
  if (peripheral >= BLFM_I2C_COUNT) {
    return BLFM_I2C_ERR_INVALID_PERIPHERAL;
  }
  if (data == NULL || len == 0) {
    return BLFM_I2C_ERR_NULL_PTR;
  }

  blfm_i2c_context_t *ctx = &i2c_contexts[peripheral];
  if (!ctx->initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  ctx->reg->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(ctx, I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (read)
  (void)ctx->reg->SR1; // Clear SB flag
  ctx->reg->DR = (addr << 1) | 0x01; // Read mode
  result = blfm_i2c_wait_event(ctx, I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  if (len == 1) {
    // Single byte read
    ctx->reg->CR1 &= ~I2C_CR1_ACK; // Disable ACK
    (void)ctx->reg->SR1; // Clear ADDR flag
    (void)ctx->reg->SR2;
    ctx->reg->CR1 |= I2C_CR1_STOP; // Generate STOP

    result = blfm_i2c_wait_event(ctx, I2C_SR1_RXNE);
    if (result != BLFM_I2C_OK) {
      ctx->reg->CR1 |= I2C_CR1_ACK; // Re-enable ACK
      return result;
    }
    data[0] = ctx->reg->DR;
  } else {
    // Multiple byte read
    ctx->reg->CR1 |= I2C_CR1_ACK; // Enable ACK
    (void)ctx->reg->SR1; // Clear ADDR flag
    (void)ctx->reg->SR2;

    for (size_t i = 0; i < len; i++) {
      if (i == len - 2) {
        ctx->reg->CR1 &= ~I2C_CR1_ACK; // Disable ACK before last byte
      }
      if (i == len - 1) {
        ctx->reg->CR1 |= I2C_CR1_STOP; // Generate STOP before last byte
      }

      result = blfm_i2c_wait_event(ctx, I2C_SR1_RXNE);
      if (result != BLFM_I2C_OK) {
        ctx->reg->CR1 |= I2C_CR1_ACK; // Re-enable ACK
        return result;
      }
      data[i] = ctx->reg->DR;
    }
  }

  ctx->reg->CR1 |= I2C_CR1_ACK; // Re-enable ACK for next transfer
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_write_byte(blfm_i2c_peripheral_t peripheral, uint8_t addr, uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  return blfm_i2c_write(peripheral, addr, buf, 2);
}

blfm_i2c_error_t blfm_i2c_read_bytes(blfm_i2c_peripheral_t peripheral, uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
  blfm_i2c_error_t result;

  // Write register address
  result = blfm_i2c_write(peripheral, addr, &reg, 1);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Read data from register
  return blfm_i2c_read(peripheral, addr, data, len);
}

blfm_i2c_error_t blfm_i2c_device_present(blfm_i2c_peripheral_t peripheral, uint8_t addr) {
  if (peripheral >= BLFM_I2C_COUNT) {
    return BLFM_I2C_ERR_INVALID_PERIPHERAL;
  }

  blfm_i2c_context_t *ctx = &i2c_contexts[peripheral];
  if (!ctx->initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  ctx->reg->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(ctx, I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (write)
  (void)ctx->reg->SR1; // Clear SB flag
  ctx->reg->DR = addr << 1; // Write mode
  result = blfm_i2c_wait_event(ctx, I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    // Device not present or not responding
    ctx->reg->CR1 |= I2C_CR1_STOP; // Generate STOP
    return result;
  }

  // Clear ADDR flag
  (void)ctx->reg->SR1;
  (void)ctx->reg->SR2;

  // Generate STOP condition
  ctx->reg->CR1 |= I2C_CR1_STOP;

  return BLFM_I2C_OK;
}