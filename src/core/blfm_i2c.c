
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_i2c.h"
#include "stm32f1xx.h"

#define I2C_TIMEOUT 10000

void blfm_i2c_init(void) {
  // Enable GPIOB and I2C1
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  // Configure PB6 (SCL) and PB7 (SDA) as Alternate Function Open-Drain
  GPIOB->CRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4))); // Clear CNF/MODE
  GPIOB->CRL |= ((0xB << (6 * 4)) |
                 (0xB << (7 * 4))); // MODE = 0b11 (50 MHz), CNF = 0b10 (AF OD)

  // Reset I2C
  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;

  I2C1->CR2 = 36;  // APB1 = 36 MHz
  I2C1->CCR = 180; // 100 kHz standard mode
  I2C1->TRISE = 37;

  I2C1->CR1 |= I2C_CR1_PE;
}

static int blfm_i2c_wait_event(uint32_t flag) {
  uint32_t timeout = I2C_TIMEOUT;
  while (!(I2C1->SR1 & flag)) {
    if (--timeout == 0)
      return -1;
  }
  return 0;
}

int blfm_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = addr << 1; // Write
  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  I2C1->DR = reg;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;

  I2C1->DR = data;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;
  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  I2C1->CR1 |= I2C_CR1_STOP;
  return 0;
}

int blfm_i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *buf, size_t len) {
  if (len == 0 || buf == NULL)
    return -1;

  // Write register address
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = addr << 1; // Write
  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;
  (void)I2C1->SR1;
  (void)I2C1->SR2;

  I2C1->DR = reg;
  if (blfm_i2c_wait_event(I2C_SR1_TXE))
    return -1;
  while (!(I2C1->SR1 & I2C_SR1_BTF)) {
  }

  // Repeated START
  I2C1->CR1 |= I2C_CR1_START;
  if (blfm_i2c_wait_event(I2C_SR1_SB))
    return -1;

  (void)I2C1->SR1;
  I2C1->DR = (addr << 1) | 0x01; // Read
  if (blfm_i2c_wait_event(I2C_SR1_ADDR))
    return -1;

  if (len == 1) {
    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP;
    while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
    }
    buf[0] = I2C1->DR;
  } else {
    I2C1->CR1 |= I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    for (size_t i = 0; i < len; i++) {
      if (i == len - 2)
        I2C1->CR1 &= ~I2C_CR1_ACK;
      if (i == len - 1)
        I2C1->CR1 |= I2C_CR1_STOP;

      while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
      }
      buf[i] = I2C1->DR;
    }
  }

  return 0;
}
