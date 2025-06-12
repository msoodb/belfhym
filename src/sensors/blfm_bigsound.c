
#include "blfm_bigsound.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx.h"
#include "blfm_config.h"
#include "blfm_exti_dispatcher.h"  // Clean dispatcher interface

#define BIGSOUND_EVENT_DETECTED 1

static QueueHandle_t bigsound_controller_queue = NULL;

// Local handler for EXTI7, registered via dispatcher
static void bigsound_exti_handler(void) {
  if (bigsound_controller_queue == NULL) {
    return;
  }

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  blfm_bigsound_event_t event;
  event.timestamp = xTaskGetTickCountFromISR();
  event.event_type = BIGSOUND_EVENT_DETECTED;

  xQueueSendFromISR(bigsound_controller_queue, &event, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void blfm_bigsound_init(QueueHandle_t controller_queue) {
  bigsound_controller_queue = controller_queue;

  // Enable GPIOA and AFIO clocks
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

  // Configure PA7 as input with pull-up
  GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
  GPIOA->CRL |= GPIO_CRL_CNF7_1; // Input mode with pull-up/pull-down
  GPIOA->ODR |= GPIO_ODR_ODR7;   // Activate pull-up

  // Map EXTI7 to PA7
  AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI7;
  AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PA;

  // Configure EXTI7 line: unmask + rising edge
  EXTI->IMR  |= EXTI_IMR_MR7;
  EXTI->RTSR |= EXTI_RTSR_TR7;

  // Register handler with the EXTI dispatcher
  blfm_exti_register_callback(7, bigsound_exti_handler);

  // Enable EXTI9_5 IRQ in NVIC (covers EXTI7)
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}
