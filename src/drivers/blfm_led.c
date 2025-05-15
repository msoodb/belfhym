
#include "blfm_led.h"
#include "FreeRTOS.h"
#include "stm32f1xx.h"
#include "task.h"

static uint16_t blink_delay_ms = 500;

void blfm_led_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
  GPIOC->CRH |= GPIO_CRH_MODE13_0; // Output, 10 MHz
  xTaskCreate(blfm_led_blink_task, "LED", 128, NULL, 1, NULL);
}

void blfm_led_set_blink_rate(uint16_t delay_ms) { blink_delay_ms = delay_ms; }

void blfm_led_blink_task(void *params) {
  (void)params;
  while (1) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    vTaskDelay(pdMS_TO_TICKS(blink_delay_ms));
  }
}
