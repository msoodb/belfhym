/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_SPI_H
#define BLFM_SPI_H

#include <stdint.h>
#include "stm32f1xx.h"
#include "blfm_pins.h"

/* SPI helper macros for NRF24 control pins */
#define CE_HIGH()   (BLFM_NRF24_CE_PORT->BSRR = (1 << BLFM_NRF24_CE_PIN))
#define CE_LOW()    (BLFM_NRF24_CE_PORT->BSRR = (1 << (BLFM_NRF24_CE_PIN + 16)))
#define CSN_HIGH()  (BLFM_NRF24_CSN_PORT->BSRR = (1 << BLFM_NRF24_CSN_PIN))
#define CSN_LOW()   (BLFM_NRF24_CSN_PORT->BSRR = (1 << (BLFM_NRF24_CSN_PIN + 16)))

/* Function declarations */
void blfm_spi_init(void);
uint8_t blfm_spi_transfer(uint8_t data);

#endif /* BLFM_SPI_H */