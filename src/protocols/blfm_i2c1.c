/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_i2c1.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"

#define I2C_TIMEOUT 10000U

static int blfm_i2c1_wait_event(uint32_t flag) {
  volatile uint32_t timeout = I2C_TIMEOUT;
  while (!(I2C1->SR1 & flag)) {
    if (--timeout == 0) {
      return -1;
    }
  }
  return 0;
}

void blfm_i2c1_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Pin config
  uint32_t scl_pos = BLFM_I2C1_SCL_PIN * 4;
  uint32_t sda_pos = BLFM_I2C1_SDA_PIN * 4;

  GPIOB->CRL &= ~((0xF << scl_pos) | (0xF << sda_pos));
  GPIOB->CRL |= ((0xB << scl_pos) | (0xB << sda_pos)); // Alternate Open-Drain

  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;

  I2C1->CR2 = 36U;  // APB1 clock MHz
  I2C1->CCR = 180U; // ~100 kHz
  I2C1->TRISE = 37U;
  I2C1->CR1 |= I2C_CR1_PE;
}

int blfm_i2c1_write(uint8_t addr, const uint8_t *data, size_t len) {
  if (!data || len == 0)
    return -1;

  // START
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c1_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = addr << 1; // Write
  if (blfm_i2c1_wait_event(I2C_SR1_ADDR))
    return -1;

  (void)I2C1->SR1;
  (void)I2C1->SR2;

  // Send all bytes
  for (size_t i = 0; i < len; i++) {
    I2C1->DR = data[i];
    if (blfm_i2c1_wait_event(I2C_SR1_TXE))
      return -1;
  }

  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  I2C1->CR1 |= I2C_CR1_STOP;

  return 0;
}

// Optional existing helpers
int blfm_i2c1_write_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  return blfm_i2c1_write(addr, buf, 2);
}

// Read with register select
int blfm_i2c1_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len) {
  if (len == 0 || buf == NULL)
    return -1;

  // Write register to set address
  if (blfm_i2c1_write(addr, &reg, 1))
    return -1;

  // Repeated START for read
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c1_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = (addr << 1) | 0x01;
  if (blfm_i2c1_wait_event(I2C_SR1_ADDR))
    return -1;

  if (len == 1) {
    // Disable ACK
    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP;

    if (blfm_i2c1_wait_event(I2C_SR1_RXNE)) {
      I2C1->CR1 |= I2C_CR1_ACK;
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
        I2C1->CR1 &= ~I2C_CR1_ACK;
      if (i == len - 1)
        I2C1->CR1 |= I2C_CR1_STOP;

      if (blfm_i2c1_wait_event(I2C_SR1_RXNE)) {
        I2C1->CR1 |= I2C_CR1_ACK;
        return -1;
      }
      buf[i] = I2C1->DR;
    }
  }

  I2C1->CR1 |= I2C_CR1_ACK;
  return 0;
}
