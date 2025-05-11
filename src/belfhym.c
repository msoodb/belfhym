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
#include "blfm_button.h"
#include "blfm_motor.h"

static uint16_t pwm = 0;

void vLedTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        if (blfm_button_is_pressed()) {
            pwm += 100;
            if (pwm > 1000) pwm = 0;
            blfm_motor_set_pwm(pwm);
            blfm_debug_print("PWM increased.\n");
            vTaskDelay(pdMS_TO_TICKS(300));  // Debounce
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


int main(void) {
    blfm_board_init();
    blfm_led_init();
    blfm_debug_init();
    blfm_button_init();
    blfm_motor_init();

    xTaskCreate(vLedTask, "LED", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1);
}

