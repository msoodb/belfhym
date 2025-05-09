/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/*#include "belfhym.h"
#include "FreeRTOS.h"
#include "task.h"

void vHelloTask(void *pvParameters) {
    while (1) {
        // Placeholder for future printf over UART or LED blink
    }
}

int main(void) {
    xTaskCreate(vHelloTask, "Hello", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1); // Should never reach here
    }*/

#include "stm32f1xx.h"


int main(void) {
    while (1) {
        // Do nothing, just loop forever
    }
}
