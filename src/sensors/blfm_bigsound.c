#include "blfm_bigsound.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx.h"

#define BIGSOUND_EVENT_DETECTED 1

static QueueHandle_t bigsound_controller_queue = NULL;

void blfm_bigsound_init(QueueHandle_t controller_queue) {
  bigsound_controller_queue = controller_queue;

  // Enable GPIOA and AFIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // Configure PA7 as input with pull-up
  GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
  GPIOA->CRL |= GPIO_CRL_CNF7_1; // Input with pull-up/pull-down
  GPIOA->ODR |= GPIO_ODR_ODR7;   // Activate pull-up

  // Map EXTI7 to PA7
  AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI7;
  AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PA;

  // Configure EXTI7: interrupt mask + rising edge
  EXTI->IMR |= EXTI_IMR_MR7;
  EXTI->RTSR |= EXTI_RTSR_TR7;

  // Enable EXTI7 interrupt in NVIC
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void blfm_bigsound_isr_handler(void) {
  if (bigsound_controller_queue == NULL) {
    return; // Queue not set up, skip
  }

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  blfm_bigsound_event_t event;
  event.timestamp = xTaskGetTickCountFromISR();
  event.event_type = BIGSOUND_EVENT_DETECTED;

  xQueueSendFromISR(bigsound_controller_queue, &event,
                    &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// EXTI9_5 IRQ handler (covers EXTI7)
void EXTI9_5_IRQHandler(void) {
  if (EXTI->PR & EXTI_PR_PR7) {
    EXTI->PR = EXTI_PR_PR7; // Clear interrupt pending flag
    blfm_bigsound_isr_handler();
  }
}
