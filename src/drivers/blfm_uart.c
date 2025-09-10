/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */


#include "blfm_uart.h"
#include "stm32f1xx.h"
#include "libc_stubs.h"


typedef struct {
    USART_TypeDef *instance;
    uint32_t baudrate;
    bool initialized;
    
    uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    volatile bool rx_overflow;
    volatile bool tx_busy;
    const uint8_t *tx_data;
    volatile uint16_t tx_count;
    volatile uint16_t tx_size;
    DMA_Channel_TypeDef *dma_tx_channel;
    DMA_Channel_TypeDef *dma_rx_channel;
    
} uart_handle_t;


static uart_handle_t uart_handles[UART_MAX_INSTANCES] = {0};

static blfm_uart_instance_t printf_uart = BLFM_UART1;


static uart_handle_t* get_uart_handle(blfm_uart_instance_t instance);
static void uart_gpio_config(blfm_uart_instance_t instance);
static void uart_clock_enable(blfm_uart_instance_t instance);
static void uart_interrupt_config(blfm_uart_instance_t instance);
static uint32_t uart_get_brr_value(uint32_t baudrate, uint32_t pclk);

static void uart_rx_interrupt_handler(uart_handle_t *handle);
static void uart_tx_interrupt_handler(uart_handle_t *handle);


blfm_uart_status_t blfm_uart_init(blfm_uart_instance_t instance, const blfm_uart_config_t *config) {
    if (instance >= UART_MAX_INSTANCES || !config) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    uart_handle_t *handle = get_uart_handle(instance);
    if (handle->initialized) {
        return BLFM_UART_ERROR_ALREADY_INITIALIZED;
    }
    
    uart_gpio_config(instance);
    uart_clock_enable(instance);
    USART_TypeDef *uart = handle->instance;
    uart->CR1 = 0;
    uart->CR2 = 0;
    uart->CR3 = 0;
    uint32_t pclk = (instance == BLFM_UART1) ? SystemCoreClock : (SystemCoreClock / 2);
    uart->BRR = uart_get_brr_value(config->baudrate, pclk);
    uint32_t cr1 = 0;
    if (config->word_length == BLFM_UART_WORDLENGTH_9B) {
        cr1 |= USART_CR1_M;
    }
    if (config->parity != BLFM_UART_PARITY_NONE) {
        cr1 |= USART_CR1_PCE;
        if (config->parity == BLFM_UART_PARITY_ODD) {
            cr1 |= USART_CR1_PS;
        }
    }
    cr1 |= USART_CR1_TE | USART_CR1_RE;
    uint32_t cr2 = 0;
    switch (config->stop_bits) {
        case BLFM_UART_STOPBITS_0_5:
            cr2 |= (0x1 << 12);
            break;
        case BLFM_UART_STOPBITS_2:
            cr2 |= (0x2 << 12);
            break;
        case BLFM_UART_STOPBITS_1_5:
            cr2 |= (0x3 << 12);
            break;
        default:
            break;
    }
    uint32_t cr3 = 0;
    if (config->hw_flow_control != BLFM_UART_HWCONTROL_NONE) {
        if (config->hw_flow_control & BLFM_UART_HWCONTROL_RTS) {
            cr3 |= USART_CR3_RTSE;
        }
        if (config->hw_flow_control & BLFM_UART_HWCONTROL_CTS) {
            cr3 |= USART_CR3_CTSE;
        }
    }
    uart->CR1 = cr1;
    uart->CR2 = cr2;
    uart->CR3 = cr3;
    handle->baudrate = config->baudrate;
    handle->rx_head = 0;
    handle->rx_tail = 0;
    handle->rx_overflow = false;
    handle->tx_busy = false;
    handle->tx_data = NULL;
    handle->tx_count = 0;
    handle->tx_size = 0;
    if (config->mode == BLFM_UART_MODE_INTERRUPT || config->mode == BLFM_UART_MODE_DMA) {
        uart_interrupt_config(instance);
        uart->CR1 |= USART_CR1_RXNEIE;
    }
    uart->CR1 |= USART_CR1_UE;
    
    handle->initialized = true;
    
    return BLFM_UART_OK;
}

blfm_uart_status_t blfm_uart_deinit(blfm_uart_instance_t instance) {
    if (instance >= UART_MAX_INSTANCES) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    uart_handle_t *handle = get_uart_handle(instance);
    if (!handle->initialized) {
        return BLFM_UART_ERROR_NOT_INITIALIZED;
    }
    
    handle->instance->CR1 = 0;
    handle->instance->CR2 = 0;
    handle->instance->CR3 = 0;
    
    handle->initialized = false;
    
    return BLFM_UART_OK;
}

blfm_uart_status_t blfm_uart_transmit_byte(blfm_uart_instance_t instance, uint8_t data) {
    uart_handle_t *handle = get_uart_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_UART_ERROR_NOT_INITIALIZED;
    }
    
    USART_TypeDef *uart = handle->instance;
    
    uint32_t timeout = UART_TIMEOUT_VALUE;
    while (!(uart->SR & USART_SR_TXE) && --timeout);
    
    if (timeout == 0) {
        return BLFM_UART_ERROR_TIMEOUT;
    }
    uart->DR = data;
    timeout = UART_TIMEOUT_VALUE;
    while (!(uart->SR & USART_SR_TC) && --timeout);
    
    return (timeout > 0) ? BLFM_UART_OK : BLFM_UART_ERROR_TIMEOUT;
}

blfm_uart_status_t blfm_uart_receive_byte(blfm_uart_instance_t instance, uint8_t *data) {
    uart_handle_t *handle = get_uart_handle(instance);
    if (!handle || !handle->initialized || !data) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    USART_TypeDef *uart = handle->instance;
    
    uint32_t timeout = UART_TIMEOUT_VALUE;
    while (!(uart->SR & USART_SR_RXNE) && --timeout);
    
    if (timeout == 0) {
        return BLFM_UART_ERROR_TIMEOUT;
    }
    *data = uart->DR;
    
    return BLFM_UART_OK;
}

blfm_uart_status_t blfm_uart_transmit(blfm_uart_instance_t instance, const uint8_t *data, uint16_t size) {
    if (!data || size == 0) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    blfm_uart_status_t status = BLFM_UART_OK;
    
    for (uint16_t i = 0; i < size; i++) {
        status = blfm_uart_transmit_byte(instance, data[i]);
        if (status != BLFM_UART_OK) {
            break;
        }
    }
    
    return status;
}

blfm_uart_status_t blfm_uart_receive(blfm_uart_instance_t instance, uint8_t *data, uint16_t size) {
    if (!data || size == 0) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    blfm_uart_status_t status = BLFM_UART_OK;
    
    for (uint16_t i = 0; i < size; i++) {
        status = blfm_uart_receive_byte(instance, &data[i]);
        if (status != BLFM_UART_OK) {
            break;
        }
    }
    
    return status;
}


blfm_uart_status_t blfm_uart_transmit_string(blfm_uart_instance_t instance, const char *str) {
    if (!str) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    return blfm_uart_transmit(instance, (const uint8_t*)str, strlen(str));
}

blfm_uart_status_t blfm_uart_receive_string(blfm_uart_instance_t instance, char *buffer, uint16_t buffer_size, char delimiter) {
    if (!buffer || buffer_size < 2) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    uint16_t index = 0;
    uint8_t received_byte;
    blfm_uart_status_t status;
    
    while (index < (buffer_size - 1)) {
        status = blfm_uart_receive_byte(instance, &received_byte);
        if (status != BLFM_UART_OK) {
            break;
        }
        
        if (received_byte == delimiter) {
            break;
        }
        
        buffer[index++] = received_byte;
    }
    
    buffer[index] = '\0';
    
    return BLFM_UART_OK;
}

int blfm_uart_printf(const char *format, ...) {
    blfm_uart_transmit_string(printf_uart, format);
    int len = 0;
    while (format[len] != '\0') len++;
    return len;
}

void blfm_uart_set_printf_uart(blfm_uart_instance_t instance) {
    if (instance < UART_MAX_INSTANCES) {
        printf_uart = instance;
    }
}

uint16_t blfm_uart_available(blfm_uart_instance_t instance) {
    uart_handle_t *handle = get_uart_handle(instance);
    if (!handle || !handle->initialized) {
        return 0;
    }
    
    if (handle->rx_head >= handle->rx_tail) {
        return handle->rx_head - handle->rx_tail;
    } else {
        return (UART_RX_BUFFER_SIZE - handle->rx_tail) + handle->rx_head;
    }
}

blfm_uart_status_t blfm_uart_read_buffer(blfm_uart_instance_t instance, uint8_t *data) {
    uart_handle_t *handle = get_uart_handle(instance);
    if (!handle || !handle->initialized || !data) {
        return BLFM_UART_ERROR_INVALID_PARAM;
    }
    
    if (handle->rx_head == handle->rx_tail) {
        return BLFM_UART_ERROR_NO_DATA;
    }
    
    *data = handle->rx_buffer[handle->rx_tail];
    handle->rx_tail = (handle->rx_tail + 1) % UART_RX_BUFFER_SIZE;
    
    return BLFM_UART_OK;
}


void USART1_IRQHandler(void) {
    uart_rx_interrupt_handler(&uart_handles[BLFM_UART1]);
    uart_tx_interrupt_handler(&uart_handles[BLFM_UART1]);
}

void USART2_IRQHandler(void) {
    uart_rx_interrupt_handler(&uart_handles[BLFM_UART2]);
    uart_tx_interrupt_handler(&uart_handles[BLFM_UART2]);
}

void USART3_IRQHandler(void) {
    uart_rx_interrupt_handler(&uart_handles[BLFM_UART3]);
    uart_tx_interrupt_handler(&uart_handles[BLFM_UART3]);
}


static uart_handle_t* get_uart_handle(blfm_uart_instance_t instance) {
    if (instance >= UART_MAX_INSTANCES) {
        return NULL;
    }
    
    uart_handle_t *handle = &uart_handles[instance];
    
    if (!handle->instance) {
        switch (instance) {
            case BLFM_UART1:
                handle->instance = USART1;
                handle->dma_tx_channel = DMA1_Channel4;
                handle->dma_rx_channel = DMA1_Channel5;
                break;
            case BLFM_UART2:
                handle->instance = USART2;
                handle->dma_tx_channel = DMA1_Channel7;
                handle->dma_rx_channel = DMA1_Channel6;
                break;
            case BLFM_UART3:
                handle->instance = USART3;
                handle->dma_tx_channel = DMA1_Channel2;
                handle->dma_rx_channel = DMA1_Channel3;
                break;
        }
    }
    
    return handle;
}

static void uart_gpio_config(blfm_uart_instance_t instance) {
    switch (instance) {
        case BLFM_UART1:
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            GPIOA->CRH &= ~((0xF << 4) | (0xF << 8));
            GPIOA->CRH |= (0xB << 4) | (0x4 << 8);
            break;
            
        case BLFM_UART2:
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            GPIOA->CRL &= ~((0xF << 8) | (0xF << 12));
            GPIOA->CRL |= (0xB << 8) | (0x4 << 12);
            break;
            
        case BLFM_UART3:
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            GPIOB->CRH &= ~((0xF << 8) | (0xF << 12));
            GPIOB->CRH |= (0xB << 8) | (0x4 << 12);
            break;
    }
}

static void uart_clock_enable(blfm_uart_instance_t instance) {
    switch (instance) {
        case BLFM_UART1:
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            break;
        case BLFM_UART2:
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
            break;
        case BLFM_UART3:
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
            break;
    }
}

static void uart_interrupt_config(blfm_uart_instance_t instance) {
    IRQn_Type irq;
    
    switch (instance) {
        case BLFM_UART1:
            irq = USART1_IRQn;
            break;
        case BLFM_UART2:
            irq = USART2_IRQn;
            break;
        case BLFM_UART3:
            irq = USART3_IRQn;
            break;
        default:
            return;
    }
    
    NVIC_SetPriority(irq, UART_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(irq);
}

static uint32_t uart_get_brr_value(uint32_t baudrate, uint32_t pclk) {
    return (pclk + (baudrate / 2)) / baudrate;
}

static void uart_rx_interrupt_handler(uart_handle_t *handle) {
    if (!handle || !handle->instance) return;
    
    USART_TypeDef *uart = handle->instance;
    
    if (uart->SR & USART_SR_RXNE) {
        uint8_t data = uart->DR;
        
        uint16_t next_head = (handle->rx_head + 1) % UART_RX_BUFFER_SIZE;
        
        if (next_head != handle->rx_tail) {
            handle->rx_buffer[handle->rx_head] = data;
            handle->rx_head = next_head;
        } else {
            handle->rx_overflow = true;
        }
    }
    if (uart->SR & (USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE)) {
        volatile uint32_t sr = uart->SR;
        volatile uint32_t dr = uart->DR;
        (void)sr; (void)dr;
    }
}

static void uart_tx_interrupt_handler(uart_handle_t *handle) {
    if (!handle || !handle->instance) return;
    
    USART_TypeDef *uart = handle->instance;
    
    if ((uart->SR & USART_SR_TXE) && (uart->CR1 & USART_CR1_TXEIE)) {
        if (handle->tx_busy && handle->tx_data && handle->tx_count < handle->tx_size) {
            uart->DR = handle->tx_data[handle->tx_count++];
            
            if (handle->tx_count >= handle->tx_size) {
                uart->CR1 &= ~USART_CR1_TXEIE;
                uart->CR1 |= USART_CR1_TCIE;
            }
        }
    }
    if ((uart->SR & USART_SR_TC) && (uart->CR1 & USART_CR1_TCIE)) {
        uart->CR1 &= ~USART_CR1_TCIE;
        handle->tx_busy = false;
        handle->tx_data = NULL;
        handle->tx_count = 0;
        handle->tx_size = 0;
    }
}