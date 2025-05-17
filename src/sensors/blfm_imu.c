
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/* MPU-6050 */

#include "blfm_imu.h"
#include "stm32f1xx.h"

#define MPU6050_ADDR 0x68
#define MPU6050_REG_PWR_MGMT 0x6B
#define MPU6050_REG_ACCEL_X 0x3B

static void i2c_write_byte(uint8_t reg, uint8_t data) {
  while (I2C1->SR2 & I2C_SR2_BUSY)
    ;
  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB))
    ;

  I2C1->DR = (MPU6050_ADDR << 1);
  while (!(I2C1->SR1 & I2C_SR1_ADDR))
    ;
  (void)I2C1->SR2;

  I2C1->DR = reg;
  while (!(I2C1->SR1 & I2C_SR1_TXE))
    ;

  I2C1->DR = data;
  while (!(I2C1->SR1 & I2C_SR1_BTF))
    ;

  I2C1->CR1 |= I2C_CR1_STOP;
}

static void i2c_read_bytes(uint8_t reg, uint8_t *buffer, uint8_t length) {
  while (I2C1->SR2 & I2C_SR2_BUSY)
    ;
  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB))
    ;

  I2C1->DR = (MPU6050_ADDR << 1);
  while (!(I2C1->SR1 & I2C_SR1_ADDR))
    ;
  (void)I2C1->SR2;

  I2C1->DR = reg;
  while (!(I2C1->SR1 & I2C_SR1_TXE))
    ;
  while (!(I2C1->SR1 & I2C_SR1_BTF))
    ;

  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB))
    ;

  I2C1->DR = (MPU6050_ADDR << 1) | 0x01;
  while (!(I2C1->SR1 & I2C_SR1_ADDR))
    ;
  (void)I2C1->SR2;

  for (uint8_t i = 0; i < length; ++i) {
    if (i == length - 1)
      I2C1->CR1 &= ~I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE))
      ;
    buffer[i] = I2C1->DR;
  }
  I2C1->CR1 |= I2C_CR1_STOP;
  I2C1->CR1 |= I2C_CR1_ACK; // re-enable ACK for next time
}

void blfm_imu_init(void) {
  i2c_write_byte(MPU6050_REG_PWR_MGMT, 0x00); // Wake up MPU6050
}

void blfm_imu_read(blfm_imu_data_t *data) {
  if (!data)
    return;

  uint8_t raw[14];
  i2c_read_bytes(MPU6050_REG_ACCEL_X, raw, 14);

  data->acc_x = (int16_t)(raw[0] << 8 | raw[1]);
  data->acc_y = (int16_t)(raw[2] << 8 | raw[3]);
  data->acc_z = (int16_t)(raw[4] << 8 | raw[5]);

  data->gyro_x = (int16_t)(raw[8] << 8 | raw[9]);
  data->gyro_y = (int16_t)(raw[10] << 8 | raw[11]);
  data->gyro_z = (int16_t)(raw[12] << 8 | raw[13]);
}
