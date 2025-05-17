
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_sensor_hub.h"
#include "FreeRTOS.h"
#include "blfm_imu.h"
#include "blfm_ultrasonic.h"
#include "semphr.h"

static blfm_sensor_data_t g_sensor_data;
static SemaphoreHandle_t sensor_data_mutex = NULL;

void blfm_sensor_hub_init(void) {
  blfm_ultrasonic_init();
  blfm_imu_init();

  if (sensor_data_mutex == NULL) {
    sensor_data_mutex = xSemaphoreCreateMutex();
    configASSERT(sensor_data_mutex != NULL);
  }
}

void blfm_sensor_hub_start(void) {
  // Placeholder for starting sensor operations if needed
  // For example: start timers or enable interrupts for sensors
}

// Optional: if you want a poll function to centralize sensor polling
void blfm_sensor_hub_poll(void) {
  // Acquire mutex before updating shared sensor data
  if (xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
    // Update ultrasonic data
    g_sensor_data.ultrasonic.distance_mm = blfm_ultrasonic_get_distance();

    // Update IMU data
    blfm_imu_read(&g_sensor_data.imu);

    xSemaphoreGive(sensor_data_mutex);
  }
}

bool blfm_sensor_hub_get_data(blfm_sensor_data_t *out_data) {
  if (!out_data)
    return false;

  if (xSemaphoreTake(sensor_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
    *out_data = g_sensor_data;
    xSemaphoreGive(sensor_data_mutex);
    return true;
  }

  return false;
}
