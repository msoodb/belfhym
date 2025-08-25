/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/**
 * @file blfm_i2c.c
 * @brief STM32F103C8T6 I2C Driver with Protocol Support
 * 
 * Complete I2C driver implementing both hardware control and protocol features:
 * - Register-level I2C control (driver layer)
 * - Device communication protocols (protocol layer)  
 * - Multi-device support with address management
 * - Error handling and recovery
 * - Common sensor protocols: MPU6050, BMP180, etc.
 */

#include "blfm_i2c.h"
#include "stm32f1xx.h"
#include <stddef.h>

/* ========================================================================== */
/*                          PRIVATE DATA STRUCTURES                          */
/* ========================================================================== */

typedef struct {
    I2C_TypeDef *instance;
    bool initialized;
    uint32_t clock_speed;
    
    /* Transfer state */
    volatile bool transfer_complete;
    volatile blfm_i2c_status_t transfer_status;
    
    /* Error tracking */
    uint32_t error_count;
    uint32_t last_error;
    
} i2c_handle_t;

/* ========================================================================== */
/*                          PRIVATE VARIABLES                                */
/* ========================================================================== */

static i2c_handle_t i2c_handles[I2C_MAX_INSTANCES] = {0};

/* ========================================================================== */
/*                          PRIVATE FUNCTION PROTOTYPES                      */
/* ========================================================================== */

static i2c_handle_t* get_i2c_handle(blfm_i2c_instance_t instance);
static void i2c_gpio_config(blfm_i2c_instance_t instance);
static void i2c_clock_enable(blfm_i2c_instance_t instance);
static blfm_i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, bool state, uint32_t timeout);
static blfm_i2c_status_t i2c_start_condition(I2C_TypeDef *i2c, uint8_t address, bool read_mode);
static void i2c_stop_condition(I2C_TypeDef *i2c);
static blfm_i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, bool read_mode);
static blfm_i2c_status_t i2c_clear_addr_flag(I2C_TypeDef *i2c);
static void i2c_reset_peripheral(blfm_i2c_instance_t instance);

/* ========================================================================== */
/*                          PUBLIC FUNCTIONS                                 */
/* ========================================================================== */

/**
 * @brief Initialize I2C instance
 */
blfm_i2c_status_t blfm_i2c_init(blfm_i2c_instance_t instance, const blfm_i2c_config_t *config) {
    if (instance >= I2C_MAX_INSTANCES || !config) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (handle->initialized) {
        return BLFM_I2C_ERROR_ALREADY_INITIALIZED;
    }
    
    /* Configure GPIO pins */
    i2c_gpio_config(instance);
    
    /* Enable peripheral clock */
    i2c_clock_enable(instance);
    
    /* Configure I2C peripheral */
    I2C_TypeDef *i2c = handle->instance;
    
    /* Reset I2C */
    i2c->CR1 = I2C_CR1_SWRST;
    i2c->CR1 = 0;
    
    /* Configure timing */
    uint32_t pclk1 = SystemCoreClock / 2;  /* APB1 clock */
    uint32_t freq = pclk1 / 1000000;       /* MHz */
    
    i2c->CR2 = freq;  /* Set peripheral clock frequency */
    
    /* Configure CCR for desired clock speed */
    uint32_t ccr_value;
    if (config->clock_speed <= 100000) {
        /* Standard mode (100kHz) */
        ccr_value = pclk1 / (2 * config->clock_speed);
        i2c->CCR = ccr_value;
        i2c->TRISE = freq + 1;
    } else {
        /* Fast mode (400kHz) */
        if (config->duty_cycle == BLFM_I2C_DUTYCYCLE_2) {
            ccr_value = pclk1 / (3 * config->clock_speed);
        } else {
            ccr_value = pclk1 / (25 * config->clock_speed);
        }
        i2c->CCR = ccr_value | I2C_CCR_FS;
        if (config->duty_cycle == BLFM_I2C_DUTYCYCLE_16_9) {
            i2c->CCR |= I2C_CCR_DUTY;
        }
        i2c->TRISE = (freq * 300) / 1000 + 1;
    }
    
    /* Configure addressing mode */
    if (config->addressing_mode == BLFM_I2C_ADDRESSINGMODE_10BIT) {
        i2c->OAR1 |= I2C_OAR1_ADDMODE;
    }
    
    /* Set own address if specified */
    if (config->own_address != 0) {
        i2c->OAR1 = (config->own_address << 1) | I2C_OAR1_ADD0;
    }
    
    /* Configure ACK */
    if (config->ack_enable) {
        i2c->CR1 |= I2C_CR1_ACK;
    }
    
    /* Enable I2C */
    i2c->CR1 |= I2C_CR1_PE;
    
    handle->clock_speed = config->clock_speed;
    handle->initialized = true;
    handle->transfer_complete = true;
    handle->transfer_status = BLFM_I2C_OK;
    handle->error_count = 0;
    handle->last_error = 0;
    
    return BLFM_I2C_OK;
}

/**
 * @brief Deinitialize I2C instance
 */
blfm_i2c_status_t blfm_i2c_deinit(blfm_i2c_instance_t instance) {
    if (instance >= I2C_MAX_INSTANCES) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (!handle->initialized) {
        return BLFM_I2C_ERROR_NOT_INITIALIZED;
    }
    
    /* Disable I2C */
    handle->instance->CR1 = 0;
    
    handle->initialized = false;
    
    return BLFM_I2C_OK;
}

/**
 * @brief Check if device is present on I2C bus
 */
blfm_i2c_status_t blfm_i2c_is_device_ready(blfm_i2c_instance_t instance, uint8_t device_address, uint32_t trials) {
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_I2C_ERROR_NOT_INITIALIZED;
    }
    
    I2C_TypeDef *i2c = handle->instance;
    blfm_i2c_status_t status = BLFM_I2C_ERROR_TIMEOUT;
    
    for (uint32_t trial = 0; trial < trials; trial++) {
        /* Wait until bus is free */
        status = i2c_wait_flag(i2c, I2C_SR2_BUSY, false, I2C_TIMEOUT_VALUE);
        if (status != BLFM_I2C_OK) {
            continue;
        }
        
        /* Generate start condition */
        i2c->CR1 |= I2C_CR1_START;
        
        /* Wait for start bit */
        status = i2c_wait_flag(i2c, I2C_SR1_SB, true, I2C_TIMEOUT_VALUE);
        if (status != BLFM_I2C_OK) {
            continue;
        }
        
        /* Send device address */
        i2c->DR = device_address & 0xFE;  /* Write operation */
        
        /* Wait for address acknowledge */
        uint32_t timeout = I2C_TIMEOUT_VALUE;
        while (timeout--) {
            uint32_t sr1 = i2c->SR1;
            
            if (sr1 & I2C_SR1_ADDR) {
                /* Address acknowledged - device is present */
                i2c_clear_addr_flag(i2c);
                i2c_stop_condition(i2c);
                return BLFM_I2C_OK;
            }
            
            if (sr1 & I2C_SR1_AF) {
                /* Address not acknowledged - device not present */
                i2c->SR1 &= ~I2C_SR1_AF;  /* Clear AF flag */
                i2c_stop_condition(i2c);
                break;
            }
        }
        
        /* Small delay between trials */
        for (volatile uint32_t i = 0; i < 1000; i++);
    }
    
    return BLFM_I2C_ERROR_NO_DEVICE;
}

/**
 * @brief Write data to I2C device (blocking)
 */
blfm_i2c_status_t blfm_i2c_write(blfm_i2c_instance_t instance, uint8_t device_address, const uint8_t *data, uint16_t size) {
    if (!data || size == 0) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_I2C_ERROR_NOT_INITIALIZED;
    }
    
    I2C_TypeDef *i2c = handle->instance;
    blfm_i2c_status_t status;
    
    /* Start condition and address */
    status = i2c_start_condition(i2c, device_address, false);
    if (status != BLFM_I2C_OK) {
        return status;
    }
    
    /* Send data */
    for (uint16_t i = 0; i < size; i++) {
        /* Wait for TXE flag */
        status = i2c_wait_flag(i2c, I2C_SR1_TXE, true, I2C_TIMEOUT_VALUE);
        if (status != BLFM_I2C_OK) {
            i2c_stop_condition(i2c);
            return status;
        }
        
        /* Send data byte */
        i2c->DR = data[i];
        
        /* Wait for BTF flag (last byte) */
        if (i == size - 1) {
            status = i2c_wait_flag(i2c, I2C_SR1_BTF, true, I2C_TIMEOUT_VALUE);
            if (status != BLFM_I2C_OK) {
                i2c_stop_condition(i2c);
                return status;
            }
        }
    }
    
    /* Generate stop condition */
    i2c_stop_condition(i2c);
    
    return BLFM_I2C_OK;
}

/**
 * @brief Read data from I2C device (blocking)
 */
blfm_i2c_status_t blfm_i2c_read(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t *data, uint16_t size) {
    if (!data || size == 0) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_I2C_ERROR_NOT_INITIALIZED;
    }
    
    I2C_TypeDef *i2c = handle->instance;
    blfm_i2c_status_t status;
    
    /* Start condition and address */
    status = i2c_start_condition(i2c, device_address, true);
    if (status != BLFM_I2C_OK) {
        return status;
    }
    
    if (size == 1) {
        /* Single byte reception */
        /* Disable ACK */
        i2c->CR1 &= ~I2C_CR1_ACK;
        
        /* Generate stop condition */
        i2c->CR1 |= I2C_CR1_STOP;
        
        /* Wait for RXNE */
        status = i2c_wait_flag(i2c, I2C_SR1_RXNE, true, I2C_TIMEOUT_VALUE);
        if (status != BLFM_I2C_OK) {
            return status;
        }
        
        /* Read data */
        data[0] = i2c->DR;
        
        /* Re-enable ACK for future transfers */
        i2c->CR1 |= I2C_CR1_ACK;
        
    } else {
        /* Multi-byte reception */
        for (uint16_t i = 0; i < size; i++) {
            if (i == size - 2) {
                /* Disable ACK before last byte */
                i2c->CR1 &= ~I2C_CR1_ACK;
            }
            
            if (i == size - 1) {
                /* Generate stop condition on last byte */
                i2c->CR1 |= I2C_CR1_STOP;
            }
            
            /* Wait for RXNE */
            status = i2c_wait_flag(i2c, I2C_SR1_RXNE, true, I2C_TIMEOUT_VALUE);
            if (status != BLFM_I2C_OK) {
                /* Re-enable ACK and return */
                i2c->CR1 |= I2C_CR1_ACK;
                return status;
            }
            
            /* Read data */
            data[i] = i2c->DR;
        }
        
        /* Re-enable ACK for future transfers */
        i2c->CR1 |= I2C_CR1_ACK;
    }
    
    return BLFM_I2C_OK;
}

/**
 * @brief Write then read from I2C device (blocking)
 */
blfm_i2c_status_t blfm_i2c_write_read(blfm_i2c_instance_t instance, uint8_t device_address, const uint8_t *write_data, uint16_t write_size, uint8_t *read_data, uint16_t read_size) {
    blfm_i2c_status_t status;
    
    /* Write phase */
    if (write_data && write_size > 0) {
        status = blfm_i2c_write(instance, device_address, write_data, write_size);
        if (status != BLFM_I2C_OK) {
            return status;
        }
    }
    
    /* Read phase */
    if (read_data && read_size > 0) {
        status = blfm_i2c_read(instance, device_address, read_data, read_size);
        if (status != BLFM_I2C_OK) {
            return status;
        }
    }
    
    return BLFM_I2C_OK;
}

/* ========================================================================== */
/*                          PROTOCOL LAYER FUNCTIONS                         */
/* ========================================================================== */

/**
 * @brief Write register to I2C device
 */
blfm_i2c_status_t blfm_i2c_write_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t value) {
    uint8_t data[2] = {reg_address, value};
    return blfm_i2c_write(instance, device_address, data, 2);
}

/**
 * @brief Read register from I2C device
 */
blfm_i2c_status_t blfm_i2c_read_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t *value) {
    if (!value) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    return blfm_i2c_write_read(instance, device_address, &reg_address, 1, value, 1);
}

/**
 * @brief Write multiple registers
 */
blfm_i2c_status_t blfm_i2c_write_registers(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t start_reg, const uint8_t *data, uint16_t length) {
    if (!data || length == 0 || length > 63) {  /* Limit to reasonable size */
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    /* Use stack buffer to avoid dynamic allocation */
    uint8_t tx_buffer[64];  /* 1 byte for register + up to 63 bytes data */
    
    tx_buffer[0] = start_reg;
    for (uint16_t i = 0; i < length; i++) {
        tx_buffer[i + 1] = data[i];
    }
    
    return blfm_i2c_write(instance, device_address, tx_buffer, length + 1);
}

/**
 * @brief Read multiple registers
 */
blfm_i2c_status_t blfm_i2c_read_registers(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t start_reg, uint8_t *data, uint16_t length) {
    if (!data || length == 0) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    return blfm_i2c_write_read(instance, device_address, &start_reg, 1, data, length);
}

/**
 * @brief Modify bits in I2C device register
 */
blfm_i2c_status_t blfm_i2c_modify_register(blfm_i2c_instance_t instance, uint8_t device_address, uint8_t reg_address, uint8_t mask, uint8_t value) {
    uint8_t current_value;
    blfm_i2c_status_t status;
    
    /* Read current value */
    status = blfm_i2c_read_register(instance, device_address, reg_address, &current_value);
    if (status != BLFM_I2C_OK) {
        return status;
    }
    
    /* Modify bits */
    uint8_t new_value = (current_value & ~mask) | (value & mask);
    
    /* Write back if changed */
    if (new_value != current_value) {
        status = blfm_i2c_write_register(instance, device_address, reg_address, new_value);
    }
    
    return status;
}

/* ========================================================================== */
/*                          COMMON SENSOR PROTOCOLS                          */
/* ========================================================================== */

/**
 * @brief MPU6050 initialization
 */
blfm_i2c_status_t blfm_i2c_mpu6050_init(blfm_i2c_instance_t instance, uint8_t device_address) {
    blfm_i2c_status_t status;
    
    /* Wake up MPU6050 */
    status = blfm_i2c_write_register(instance, device_address, MPU6050_PWR_MGMT_1, 0x00);
    if (status != BLFM_I2C_OK) return status;
    
    /* Configure sample rate divider */
    status = blfm_i2c_write_register(instance, device_address, MPU6050_SMPLRT_DIV, 0x07);
    if (status != BLFM_I2C_OK) return status;
    
    /* Configure accelerometer (±2g) */
    status = blfm_i2c_write_register(instance, device_address, MPU6050_ACCEL_CONFIG, 0x00);
    if (status != BLFM_I2C_OK) return status;
    
    /* Configure gyroscope (±250°/s) */
    status = blfm_i2c_write_register(instance, device_address, MPU6050_GYRO_CONFIG, 0x00);
    
    return status;
}

/**
 * @brief MPU6050 read accelerometer data
 */
blfm_i2c_status_t blfm_i2c_mpu6050_read_accel(blfm_i2c_instance_t instance, uint8_t device_address, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z) {
    if (!accel_x || !accel_y || !accel_z) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    uint8_t data[6];
    blfm_i2c_status_t status = blfm_i2c_read_registers(instance, device_address, MPU6050_ACCEL_XOUT_H, data, 6);
    
    if (status == BLFM_I2C_OK) {
        *accel_x = (int16_t)((data[0] << 8) | data[1]);
        *accel_y = (int16_t)((data[2] << 8) | data[3]);
        *accel_z = (int16_t)((data[4] << 8) | data[5]);
    }
    
    return status;
}

/**
 * @brief MPU6050 read gyroscope data
 */
blfm_i2c_status_t blfm_i2c_mpu6050_read_gyro(blfm_i2c_instance_t instance, uint8_t device_address, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z) {
    if (!gyro_x || !gyro_y || !gyro_z) {
        return BLFM_I2C_ERROR_INVALID_PARAM;
    }
    
    uint8_t data[6];
    blfm_i2c_status_t status = blfm_i2c_read_registers(instance, device_address, MPU6050_GYRO_XOUT_H, data, 6);
    
    if (status == BLFM_I2C_OK) {
        *gyro_x = (int16_t)((data[0] << 8) | data[1]);
        *gyro_y = (int16_t)((data[2] << 8) | data[3]);
        *gyro_z = (int16_t)((data[4] << 8) | data[5]);
    }
    
    return status;
}

/* ========================================================================== */
/*                          PRIVATE FUNCTIONS                                */
/* ========================================================================== */

static i2c_handle_t* get_i2c_handle(blfm_i2c_instance_t instance) {
    if (instance >= I2C_MAX_INSTANCES) {
        return NULL;
    }
    
    i2c_handle_t *handle = &i2c_handles[instance];
    
    /* Initialize instance pointer if not set */
    if (!handle->instance) {
        switch (instance) {
            case BLFM_I2C1:
                handle->instance = I2C1;
                break;
            case BLFM_I2C2:
                handle->instance = I2C2;
                break;
        }
    }
    
    return handle;
}

static void i2c_gpio_config(blfm_i2c_instance_t instance) {
    switch (instance) {
        case BLFM_I2C1:
            /* PB6=SCL, PB7=SDA (AF Open-drain) */
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
            GPIOB->CRL |= (0xF << 24) | (0xF << 28);
            break;
            
        case BLFM_I2C2:
            /* PB10=SCL, PB11=SDA (AF Open-drain) */
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            GPIOB->CRH &= ~((0xF << 8) | (0xF << 12));
            GPIOB->CRH |= (0xF << 8) | (0xF << 12);
            break;
    }
}

static void i2c_clock_enable(blfm_i2c_instance_t instance) {
    switch (instance) {
        case BLFM_I2C1:
            RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
            break;
        case BLFM_I2C2:
            RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
            break;
    }
}

static blfm_i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, bool state, uint32_t timeout) {
    while (timeout--) {
        bool flag_set;
        
        if (flag & 0xFF0000) {
            /* SR2 flags */
            flag_set = (i2c->SR2 & (flag >> 16)) != 0;
        } else {
            /* SR1 flags */
            flag_set = (i2c->SR1 & flag) != 0;
        }
        
        if (flag_set == state) {
            return BLFM_I2C_OK;
        }
    }
    
    return BLFM_I2C_ERROR_TIMEOUT;
}

static blfm_i2c_status_t i2c_start_condition(I2C_TypeDef *i2c, uint8_t address, bool read_mode) {
    blfm_i2c_status_t status;
    
    /* Wait until bus is free */
    status = i2c_wait_flag(i2c, I2C_SR2_BUSY, false, I2C_TIMEOUT_VALUE);
    if (status != BLFM_I2C_OK) {
        return status;
    }
    
    /* Generate start condition */
    i2c->CR1 |= I2C_CR1_START;
    
    /* Wait for start bit */
    status = i2c_wait_flag(i2c, I2C_SR1_SB, true, I2C_TIMEOUT_VALUE);
    if (status != BLFM_I2C_OK) {
        return status;
    }
    
    /* Send address */
    status = i2c_send_address(i2c, address, read_mode);
    if (status != BLFM_I2C_OK) {
        i2c_stop_condition(i2c);
        return status;
    }
    
    return BLFM_I2C_OK;
}

static void i2c_stop_condition(I2C_TypeDef *i2c) {
    i2c->CR1 |= I2C_CR1_STOP;
}

static blfm_i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, bool read_mode) {
    /* Send address with R/W bit */
    i2c->DR = address | (read_mode ? 1 : 0);
    
    /* Wait for address acknowledge */
    blfm_i2c_status_t status = i2c_wait_flag(i2c, I2C_SR1_ADDR, true, I2C_TIMEOUT_VALUE);
    if (status != BLFM_I2C_OK) {
        return BLFM_I2C_ERROR_NO_DEVICE;
    }
    
    /* Clear address flag */
    return i2c_clear_addr_flag(i2c);
}

static blfm_i2c_status_t i2c_clear_addr_flag(I2C_TypeDef *i2c) {
    /* Clear ADDR flag by reading SR1 then SR2 */
    volatile uint32_t sr1 = i2c->SR1;
    volatile uint32_t sr2 = i2c->SR2;
    (void)sr1; (void)sr2;
    
    return BLFM_I2C_OK;
}

__attribute__((unused))
static void i2c_reset_peripheral(blfm_i2c_instance_t instance) {
    i2c_handle_t *handle = get_i2c_handle(instance);
    if (!handle || !handle->instance) {
        return;
    }
    
    I2C_TypeDef *i2c = handle->instance;
    
    /* Software reset */
    i2c->CR1 |= I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_SWRST;
    
    /* Re-enable peripheral */
    i2c->CR1 |= I2C_CR1_PE;
    
    handle->error_count++;
}