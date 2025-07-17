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
#include <stdbool.h>

#define I2C_DEFAULT_TIMEOUT_MS 100
#define I2C_DEFAULT_SPEED_HZ 100000  // 100 kHz
#define I2C_APB1_FREQ_MHZ 36         // APB1 clock frequency

static uint32_t timeout_counter = 10000;
static bool initialized = false;

/**
 * @brief Wait for I2C event with timeout
 * @param flag Event flag to wait for
 * @return BLFM_I2C_OK on success, BLFM_I2C_ERR_TIMEOUT on timeout
 */
static blfm_i2c_error_t blfm_i2c_wait_event(uint32_t flag) {
  volatile uint32_t timeout = timeout_counter;
  while (!(I2C1->SR1 & flag)) {
    if (--timeout == 0) {
      return BLFM_I2C_ERR_TIMEOUT;
    }
  }
  return BLFM_I2C_OK;
}

/**
 * @brief Configure GPIO pins for I2C1 (PB6=SCL, PB7=SDA)
 */
static void blfm_i2c_configure_gpio(void) {
  // Enable GPIOB and alternate function clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

  // Configure PB6 (SCL) and PB7 (SDA) as alternate function open-drain
  // PB6 position: 6 * 4 = 24, PB7 position: 7 * 4 = 28
  GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
  GPIOB->CRL |= (0xB << 24) | (0xB << 28); // Alternate Open-Drain, 50MHz
}

blfm_i2c_error_t blfm_i2c_init(const blfm_i2c_config_t *config) {
  // Use default config if none provided
  blfm_i2c_config_t default_config = {
    .speed_hz = I2C_DEFAULT_SPEED_HZ,
    .timeout_ms = I2C_DEFAULT_TIMEOUT_MS
  };
  
  if (config == NULL) {
    config = &default_config;
  }

  // Update timeout counter based on config
  timeout_counter = config->timeout_ms * 100; // Rough approximation

  // Enable I2C1 peripheral clock
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Configure GPIO pins
  blfm_i2c_configure_gpio();

  // Reset I2C peripheral
  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;

  // Configure I2C
  I2C1->CR2 = I2C_APB1_FREQ_MHZ;

  // Calculate CCR value based on desired speed
  uint32_t ccr_value;
  if (config->speed_hz <= 100000) {
    // Standard mode (up to 100 kHz)
    ccr_value = (I2C_APB1_FREQ_MHZ * 1000000) / (2 * config->speed_hz);
    I2C1->CCR = ccr_value;
    I2C1->TRISE = I2C_APB1_FREQ_MHZ + 1;
  } else {
    // Fast mode (up to 400 kHz)
    ccr_value = (I2C_APB1_FREQ_MHZ * 1000000) / (3 * config->speed_hz);
    I2C1->CCR = ccr_value | I2C_CCR_FS;
    I2C1->TRISE = ((I2C_APB1_FREQ_MHZ * 300) / 1000) + 1;
  }

  // Enable I2C peripheral
  I2C1->CR1 |= I2C_CR1_PE;

  initialized = true;
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_deinit(void) {
  // Disable I2C peripheral
  I2C1->CR1 = 0;

  // Disable peripheral clock
  RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;

  initialized = false;
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_write(uint8_t addr, const uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    return BLFM_I2C_ERR_NULL_PTR;
  }

  if (!initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  I2C1->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (write)
  (void)I2C1->SR1; // Clear SB flag
  I2C1->DR = addr << 1; // Write mode
  result = blfm_i2c_wait_event(I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Clear ADDR flag
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  // Send data bytes
  for (size_t i = 0; i < len; i++) {
    I2C1->DR = data[i];
    result = blfm_i2c_wait_event(I2C_SR1_TXE);
    if (result != BLFM_I2C_OK) {
      return result;
    }
  }

  // Wait for transfer complete
  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
    // Wait for byte transfer finished
  }

  // Generate STOP condition
  I2C1->CR1 |= I2C_CR1_STOP;

  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_read(uint8_t addr, uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    return BLFM_I2C_ERR_NULL_PTR;
  }

  if (!initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  I2C1->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (read)
  (void)I2C1->SR1; // Clear SB flag
  I2C1->DR = (addr << 1) | 0x01; // Read mode
  result = blfm_i2c_wait_event(I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  if (len == 1) {
    // Single byte read
    I2C1->CR1 &= ~I2C_CR1_ACK; // Disable ACK
    (void)I2C1->SR1; // Clear ADDR flag
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP; // Generate STOP

    result = blfm_i2c_wait_event(I2C_SR1_RXNE);
    if (result != BLFM_I2C_OK) {
      I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK
      return result;
    }
    data[0] = I2C1->DR;
  } else {
    // Multiple byte read
    I2C1->CR1 |= I2C_CR1_ACK; // Enable ACK
    (void)I2C1->SR1; // Clear ADDR flag
    (void)I2C1->SR2;

    for (size_t i = 0; i < len; i++) {
      if (i == len - 2) {
        I2C1->CR1 &= ~I2C_CR1_ACK; // Disable ACK before last byte
      }
      if (i == len - 1) {
        I2C1->CR1 |= I2C_CR1_STOP; // Generate STOP before last byte
      }

      result = blfm_i2c_wait_event(I2C_SR1_RXNE);
      if (result != BLFM_I2C_OK) {
        I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK
        return result;
      }
      data[i] = I2C1->DR;
    }
  }

  I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK for next transfer
  return BLFM_I2C_OK;
}

blfm_i2c_error_t blfm_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  return blfm_i2c_write(addr, buf, 2);
}

blfm_i2c_error_t blfm_i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
  blfm_i2c_error_t result;

  // Write register address
  result = blfm_i2c_write(addr, &reg, 1);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Read data from register
  return blfm_i2c_read(addr, data, len);
}

blfm_i2c_error_t blfm_i2c_device_present(uint8_t addr) {
  if (!initialized) {
    return BLFM_I2C_ERR_NOT_INITIALIZED;
  }

  blfm_i2c_error_t result;

  // Generate START condition
  I2C1->CR1 |= I2C_CR1_START;
  result = blfm_i2c_wait_event(I2C_SR1_SB);
  if (result != BLFM_I2C_OK) {
    return result;
  }

  // Send device address (write)
  (void)I2C1->SR1; // Clear SB flag
  I2C1->DR = addr << 1; // Write mode
  result = blfm_i2c_wait_event(I2C_SR1_ADDR);
  if (result != BLFM_I2C_OK) {
    // Device not present or not responding
    I2C1->CR1 |= I2C_CR1_STOP; // Generate STOP
    return result;
  }

  // Clear ADDR flag
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  // Generate STOP condition
  I2C1->CR1 |= I2C_CR1_STOP;

  return BLFM_I2C_OK;
}