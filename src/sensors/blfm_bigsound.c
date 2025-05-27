
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_bigsound.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"

static TaskHandle_t controller_task_handle = NULL;

#define SOUND_PORT GPIOB
#define SOUND_PIN 7

void blfm_bigsound_register_controller_task(TaskHandle_t controller_handle) {
  controller_task_handle = controller_handle;
}

void blfm_bigsound_init(void) {
  // Configure GPIO as input
  blfm_gpio_config_input((uint32_t)SOUND_PORT, SOUND_PIN);

  // Enable AFIO clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Map EXTI line 5 to PB5
  AFIO->EXTICR[1] &= ~(AFIO_EXTICR2_EXTI5);
  AFIO->EXTICR[1] |= (AFIO_EXTICR2_EXTI5_PB);

  // Configure EXTI line 5 for rising edge trigger
  EXTI->IMR |= EXTI_IMR_MR5;    // Unmask interrupt
  EXTI->RTSR |= EXTI_RTSR_TR5;  // Enable rising edge
  EXTI->FTSR &= ~EXTI_FTSR_TR5; // Disable falling edge

  // Enable EXTI9_5 interrupt in NVIC
  NVIC_SetPriority(EXTI9_5_IRQn, 5);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// ISR: EXTI lines 9 to 5
void EXTI9_5_IRQHandler(void) {
  if (EXTI->PR & EXTI_PR_PR5) {
    EXTI->PR |= EXTI_PR_PR5; // Clear pending bit

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (controller_task_handle != NULL) {
      vTaskNotifyGiveFromISR(controller_task_handle, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
