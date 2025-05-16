
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_taskmanager.h"
#include "FreeRTOS.h"
#include "task.h"

void vSensorTask(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    // Simulate reading sensor
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vMotorTask(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    // Simulate motor control
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vSafetyTask(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    // Simulate safety monitoring
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void blfm_taskmanager_setup(void) {
  xTaskCreate(vSensorTask, "Sensor", 128, NULL, 2, NULL);
  xTaskCreate(vMotorTask, "Motor", 128, NULL, 2, NULL);
  xTaskCreate(vSafetyTask, "Safety", 128, NULL, 3, NULL);
}

void blfm_taskmanager_start(void) { vTaskStartScheduler(); }
