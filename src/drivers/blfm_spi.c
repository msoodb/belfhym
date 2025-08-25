/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_spi.h"

void blfm_spi_init(void) {
    /* Enable clocks for GPIOA and SPI1 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;
    
    /* Configure SPI pins */
    /* PA5 (SCK) - Alternate function push-pull */
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_SCK_PIN * 4));
    GPIOA->CRL |= (0xB << (BLFM_SPI1_SCK_PIN * 4));
    
    /* PA6 (MISO) - Input floating */
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_MISO_PIN * 4));
    GPIOA->CRL |= (0x4 << (BLFM_SPI1_MISO_PIN * 4));
    
    /* PA7 (MOSI) - Alternate function push-pull */
    GPIOA->CRL &= ~(0xF << (BLFM_SPI1_MOSI_PIN * 4));
    GPIOA->CRL |= (0xB << (BLFM_SPI1_MOSI_PIN * 4));
    
    /* Configure NRF24 control pins */
    /* PA4 (CE) - Output push-pull */
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_CE_PIN * 4));
    GPIOA->CRL |= (0x3 << (BLFM_NRF24_CE_PIN * 4));
    
    /* PA0 (CSN) - Output push-pull */
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_CSN_PIN * 4));
    GPIOA->CRL |= (0x3 << (BLFM_NRF24_CSN_PIN * 4));
    
    /* PA1 (IRQ) - Input with pull-up */
    GPIOA->CRL &= ~(0xF << (BLFM_NRF24_IRQ_PIN * 4));
    GPIOA->CRL |= (0x8 << (BLFM_NRF24_IRQ_PIN * 4));
    GPIOA->BSRR = (1 << BLFM_NRF24_IRQ_PIN);  /* Enable pull-up */
    
    /* Initialize control pins to default state */
    CSN_HIGH();
    CE_LOW();
    
    /* Configure SPI1 */
    /* Master mode, BR=fPCLK/8 (9MHz), CPOL=0, CPHA=0, 8-bit, MSB first */
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSM | SPI_CR1_SSI;
    
    /* Enable SPI1 */
    SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t blfm_spi_transfer(uint8_t data) {
    /* Wait for transmit buffer empty */
    while (!(SPI1->SR & SPI_SR_TXE));
    
    /* Send data */
    SPI1->DR = data;
    
    /* Wait for receive buffer not empty */
    while (!(SPI1->SR & SPI_SR_RXNE));
    
    /* Return received data */
    return SPI1->DR;
}