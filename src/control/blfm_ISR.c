#include "blfm_ISR.h"
#include "stm32f1xx.h" // CMSIS header

static TaskHandle_t controller_task_handle = NULL;

void blfm_isr_register_controller_task(TaskHandle_t controller_handle) {
  controller_task_handle = controller_handle;
}

void blfm_isr_init(void) {
  // Example: Configure EXTI line for bigsound sensor on GPIO pin (say, PA0)
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Enable GPIOA clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Enable AFIO clock

  // Configure PA0 as input with pull-up/down (assuming bigsound on PA0)
  GPIOA->CRL &= ~GPIO_CRL_MODE0;
  GPIOA->CRL &= ~GPIO_CRL_CNF0;
  GPIOA->CRL |= GPIO_CRL_CNF0_1; // Input with pull-up/pull-down

  // Enable pull-up
  GPIOA->ODR |= GPIO_ODR_ODR0;

  // Connect EXTI0 to PA0
  AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0; // Clear
  AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;

  // Configure EXTI0 line
  EXTI->IMR |= EXTI_IMR_MR0;   // Unmask interrupt
  EXTI->FTSR |= EXTI_FTSR_TR0; // Trigger on falling edge

  // Enable EXTI0 interrupt in NVIC
  NVIC_EnableIRQ(EXTI0_IRQn);
}

// Interrupt handler for EXTI0 (bigsound)
void EXTI0_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (EXTI->PR & EXTI_PR_PR0) {
    EXTI->PR |= EXTI_PR_PR0; // Clear pending bit

    if (controller_task_handle != NULL) {
      // Send notification to ControllerTask
      vTaskNotifyGiveFromISR(controller_task_handle, &xHigherPriorityTaskWoken);
    }
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
