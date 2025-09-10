/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_spi.h"

void blfm_spi_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_SCK_PIN * 4));
    GPIOA->CRL |= (0xB << (BLFM_SPI1_SCK_PIN * 4));
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_MISO_PIN * 4));
    GPIOA->CRL |= (0x4 << (BLFM_SPI1_MISO_PIN * 4));
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_MOSI_PIN * 4));
    GPIOA->CRL |= (0xB << (BLFM_SPI1_MOSI_PIN * 4));
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_CE_PIN * 4));
    GPIOA->CRL |= (0x3 << (BLFM_NRF24_CE_PIN * 4));
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_CSN_PIN * 4));
    GPIOA->CRL |= (0x3 << (BLFM_NRF24_CSN_PIN * 4));
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_IRQ_PIN * 4));
    GPIOA->CRL |= (0x8 << (BLFM_NRF24_IRQ_PIN * 4));
    GPIOA->BSRR = (1 << BLFM_NRF24_IRQ_PIN);
    CSN_HIGH();
    CE_LOW();
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t blfm_spi_transfer(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}