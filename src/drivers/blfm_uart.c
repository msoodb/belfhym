
#include "blfm_uart.h"
#include "stm32f1xx.h"

void blfm_uart_init(void) {
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1);

    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    USART1->BRR = SystemCoreClock / 115200;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
}

void blfm_uart_write(const char *msg, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = msg[i];
    }
}
