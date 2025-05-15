
#include "blfm_board.h"
#include "stm32f1xx.h"

void blfm_board_init(void) {
    SystemInit();
    SystemCoreClockUpdate();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
}
