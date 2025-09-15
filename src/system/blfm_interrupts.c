
#include "blfm_interrupts.h"
#include "stm32f4xx.h" 

#define MAX_EXTI_LINES 16

static blfm_exti_callback_t exti_callbacks[MAX_EXTI_LINES] = {0};


void blfm_exti_init(uint8_t gpio_port, uint8_t pin, blfm_exti_trigger_t trigger, blfm_exti_callback_t callback) {
  if (pin >= MAX_EXTI_LINES) return;
  
  
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  
  uint8_t exti_idx = pin / 4;
  uint8_t exti_shift = (pin % 4) * 4;
  SYSCFG->EXTICR[exti_idx] &= ~(0xFU << exti_shift);
  SYSCFG->EXTICR[exti_idx] |= (gpio_port << exti_shift);
  
  
  if (trigger & BLFM_EXTI_TRIGGER_RISING) {
    EXTI->RTSR |= (1U << pin);
  } else {
    EXTI->RTSR &= ~(1U << pin);
  }
  
  if (trigger & BLFM_EXTI_TRIGGER_FALLING) {
    EXTI->FTSR |= (1U << pin);
  } else {
    EXTI->FTSR &= ~(1U << pin);
  }
  
  
  EXTI->IMR |= (1U << pin);
  
  
  exti_callbacks[pin] = callback;
  
  
  if (pin == 0) {
    NVIC_EnableIRQ(EXTI0_IRQn);
  } else if (pin == 1) {
    NVIC_EnableIRQ(EXTI1_IRQn);
  } else if (pin == 2) {
    NVIC_EnableIRQ(EXTI2_IRQn);
  } else if (pin == 3) {
    NVIC_EnableIRQ(EXTI3_IRQn);
  } else if (pin == 4) {
    NVIC_EnableIRQ(EXTI4_IRQn);
  } else if (pin >= 5 && pin <= 9) {
    NVIC_EnableIRQ(EXTI9_5_IRQn);
  } else if (pin >= 10 && pin <= 15) {
    NVIC_EnableIRQ(EXTI15_10_IRQn);
  }
}


void blfm_exti_register_callback(uint8_t exti_line, blfm_exti_callback_t callback) {
  if (exti_line < MAX_EXTI_LINES) {
    exti_callbacks[exti_line] = callback;
  }
}


void EXTI0_IRQHandler(void) {
  if (EXTI->PR & (1U << 0)) {
    EXTI->PR = (1U << 0);
    if (exti_callbacks[0]) {
      exti_callbacks[0]();
    }
  }
}

void EXTI1_IRQHandler(void) {
  if (EXTI->PR & (1U << 1)) {
    EXTI->PR = (1U << 1);
    if (exti_callbacks[1]) {
      exti_callbacks[1]();
    }
  }
}

void EXTI2_IRQHandler(void) {
  if (EXTI->PR & (1U << 2)) {
    EXTI->PR = (1U << 2);
    if (exti_callbacks[2]) {
      exti_callbacks[2]();
    }
  }
}

void EXTI3_IRQHandler(void) {
  if (EXTI->PR & (1U << 3)) {
    EXTI->PR = (1U << 3);
    if (exti_callbacks[3]) {
      exti_callbacks[3]();
    }
  }
}

void EXTI4_IRQHandler(void) {
  if (EXTI->PR & (1U << 4)) {
    EXTI->PR = (1U << 4);
    if (exti_callbacks[4]) {
      exti_callbacks[4]();
    }
  }
}

void EXTI9_5_IRQHandler(void) {
  uint32_t pending = EXTI->PR & 0x03E0;
  EXTI->PR = pending;
  
  for (uint8_t line = 5; line <= 9; line++) {
    if ((pending & (1U << line)) && exti_callbacks[line]) {
      exti_callbacks[line]();
    }
  }
}

void EXTI15_10_IRQHandler(void) {
  uint32_t pending = EXTI->PR & 0xFC00;
  EXTI->PR = pending;
  
  for (uint8_t line = 10; line <= 15; line++) {
    if ((pending & (1U << line)) && exti_callbacks[line]) {
      exti_callbacks[line]();
    }
  }
}
