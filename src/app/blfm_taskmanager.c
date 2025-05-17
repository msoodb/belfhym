/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/**
 * @file blfm_taskmanager.c
 * @brief FreeRTOS task setup and startup manager.
 *
 * Registers and launches key tasks, such as sensor polling,
 * actuator control, and safety monitoring.
 */

#include "blfm_taskmanager.h"
#include "FreeRTOS.h"
#include "task.h"

#include "blfm_sensor_hub.h"
#include "blfm_actuator_hub.h"  // Actuator hub managing motors, alarms, etc.

// Sensor Hub Task: runs sensor hub polling if needed
static void vSensorHubTask(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        blfm_sensor_hub_poll();  // Centralized polling of all sensors
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Actuator Hub Task: runs actuator hub processing
static void vActuatorHubTask(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        // TODO: actuator hub logic: motor control, alarm triggering, etc.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void blfm_taskmanager_setup(void) {
    blfm_sensor_hub_init();
    blfm_actuator_hub_init();

    xTaskCreate(vSensorHubTask, "SensorHub", 128, NULL, 2, NULL);
    xTaskCreate(vActuatorHubTask, "ActuatorHub", 128, NULL, 2, NULL);
}

void blfm_taskmanager_start(void) {
    vTaskStartScheduler();
}
