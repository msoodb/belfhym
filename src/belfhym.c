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

#include "stm32f1xx.h"

#include "blfm_board.h"
#include "blfm_led.h"
#include "blfm_debug.h"

void vLedTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        blfm_led_toggle();
        blfm_debug_print("LED toggled\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    blfm_board_init();
    blfm_led_init();
    blfm_debug_init();

    xTaskCreate(vLedTask, "LED", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1);
}

