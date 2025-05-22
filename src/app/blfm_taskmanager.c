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
#include "blfm_actuator_hub.h"
#include "blfm_controller.h"
#include "blfm_sensor_hub.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f1xx.h"

#define SENSOR_HUB_TASK_STACK 128
#define ACTUATOR_HUB_TASK_STACK 128
#define CONTROLLER_TASK_STACK 128

#define SENSOR_HUB_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 3


static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    blfm_sensor_hub_poll();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    blfm_actuator_hub_update();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    blfm_controller_run();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void blfm_taskmanager_setup(void) {
  // Initialize modules
  //blfm_sensor_hub_init();
  blfm_actuator_hub_init();
  //blfm_controller_init();
  
  // Start tasks inside modules
  //blfm_sensor_hub_start();
  blfm_actuator_hub_start();
  //blfm_controller_start();     // if you split controller similarly

  // Create tasks
  /*xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
    SENSOR_HUB_TASK_PRIORITY, NULL);*/
  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);
  /*xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
    CONTROLLER_TASK_PRIORITY, NULL);*/
}

void blfm_taskmanager_start(void) { vTaskStartScheduler(); }
