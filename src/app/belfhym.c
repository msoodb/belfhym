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
#include "blfm_alarm.h"
#include "blfm_led.h"
#include "blfm_motor.h"
#include "blfm_safety.h"
#include "blfm_thermal.h"
#include "blfm_ultrasonic.h"
#include "task.h"

int main(void) {
  blfm_led_init();
  blfm_motor_init();
  blfm_ultrasonic_init();
  blfm_thermal_init();
  blfm_alarm_init();

  xTaskCreate(blfm_safety_monitor_task, "Safety", 128, NULL, 2, NULL);
  xTaskCreate(blfm_led_blink_task, "LED", 128, NULL, 1, NULL);

  vTaskStartScheduler();
  while (1)
    ;
}
