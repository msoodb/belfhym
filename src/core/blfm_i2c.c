/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_i2c.h"
#include "stm32f1xx.h" // CMSIS device header

#define I2C_TIMEOUT 10000U

void blfm_i2c_init(void) {
  // Enable GPIOB clock and AFIO clock (needed for alternate function remap)
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

  // Enable I2C1 clock
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Configure PB6 (SCL) and PB7 (SDA) as Alternate Function Open-Drain, max
  // speed 50 MHz Each pin is configured in 4 bits: MODE[1:0] and CNF[1:0] MODE
  // = 0b11 (Output mode, max speed 50 MHz) CNF = 0b10 (Alternate function
  // Open-Drain)
  GPIOB->CRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4))); // Clear bits
  GPIOB->CRL |= ((0xB << (6 * 4)) | (0xB << (7 * 4))); // Set bits: 0xB = 0b1011

  // Reset I2C peripheral
  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;

  // Configure frequency bits in CR2 (APB1 clock freq in MHz)
  I2C1->CR2 = 36U; // 36 MHz APB1 clock

  // Configure clock control register for 100 kHz standard mode
  // CCR = Fpclk1 / (2 * Fscl) = 36 MHz / (2 * 100kHz) = 180
  I2C1->CCR = 180U;

  // Configure maximum rise time (TRISE) = Fpclk1 (MHz) + 1
  I2C1->TRISE = 37U;

  // Enable I2C peripheral
  I2C1->CR1 |= I2C_CR1_PE;
}

static int blfm_i2c_wait_event(uint32_t flag) {
  volatile uint32_t timeout = I2C_TIMEOUT;
  while (!(I2C1->SR1 & flag)) {
    if (--timeout == 0) {
      return -1; // timeout
    }
  }
  return 0; // success
}

int blfm_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  // START condition
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;      // Clear SB by reading SR1 followed by writing DR
  I2C1->DR = addr << 1; // Write mode (R/W bit = 0)

  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;

  (void)I2C1->SR1; // Clear ADDR flag by reading SR1 and SR2
  (void)I2C1->SR2;

  // Send register address
  I2C1->DR = reg;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;

  // Send data byte
  I2C1->DR = data;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;

  // Wait for byte transfer finished
  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  // STOP condition
  I2C1->CR1 |= I2C_CR1_STOP;

  return 0;
}

int blfm_i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len) {
  if (len == 0 || buf == NULL)
    return -1;

  // Write register address (to select register for reading)
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = addr << 1; // Write mode

  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;

  (void)I2C1->SR1;
  (void)I2C1->SR2;

  I2C1->DR = reg;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;

  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  // Repeated START for read
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = (addr << 1) | 0x01; // Read mode

  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;

  if (len == 1) {
    // Disable ACK, clear ADDR, generate STOP
    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP;

    if (blfm_i2c_wait_event(I2C_SR1_RXNE)) {
      I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK for next transfer
      return -1;
    }
    buf[0] = I2C1->DR;
  } else {
    // Enable ACK
    I2C1->CR1 |= I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    for (size_t i = 0; i < len; i++) {
      if (i == len - 2)
        I2C1->CR1 &= ~I2C_CR1_ACK; // NACK next byte
      if (i == len - 1)
        I2C1->CR1 |= I2C_CR1_STOP; // Generate STOP

      if (blfm_i2c_wait_event(I2C_SR1_RXNE)) {
        I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK if error
        return -1;
      }
      buf[i] = I2C1->DR;
    }
  }

  I2C1->CR1 |= I2C_CR1_ACK; // Re-enable ACK after operation

  return 0;
}

int blfm_i2c_write_byte_simple(uint8_t addr, uint8_t data) {
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = addr << 1;

  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;

  (void)I2C1->SR1;
  (void)I2C1->SR2;

  I2C1->DR = data;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;

  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  I2C1->CR1 |= I2C_CR1_STOP;

  return 0;
}
