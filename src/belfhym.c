/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "belfhym.h"
#include "FreeRTOS.h"
#include "task.h"
#include "blfm_board.h"

/*
  Blink the LED in vHelloTask
  The onboard LED on Blue Pill is usually connected to PC13.
  Add GPIO initialization for PC13, then toggle it inside vHelloTask.
*/
void vHelloTask(void *pvParameters) {
    blfm_board_led_init();

    while (1) {
        blfm_board_led_toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
  // Optional future initialization: system clocks, peripherals, etc.
  // blfm_board_init();

  // Create LED blink task
  xTaskCreate(vHelloTask, "Hello", 128, NULL, 1, NULL);

  // Start FreeRTOS scheduler
  vTaskStartScheduler();

  // Should never reach here
  while (1)
    ;
}

