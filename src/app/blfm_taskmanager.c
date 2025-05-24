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
#include "blfm_types.h"

#include "blfm_debug.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// --- Task Configuration ---
#define SENSOR_HUB_TASK_STACK 256
#define CONTROLLER_TASK_STACK 256
#define ACTUATOR_HUB_TASK_STACK 256

#define SENSOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2

// --- Queues ---
static QueueHandle_t xSensorDataQueue = NULL;
static QueueHandle_t xActuatorCmdQueue = NULL;

// --- Task Prototypes ---
static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);

// --- Public Setup Function ---
void blfm_taskmanager_setup(void) {
  // Initialize all modules
  blfm_sensor_hub_init();
  blfm_controller_init();
  blfm_actuator_hub_init();

  // Create queues
  xSensorDataQueue = xQueueCreate(5, sizeof(blfm_sensor_data_t));
  configASSERT(xSensorDataQueue != NULL);

  xActuatorCmdQueue = xQueueCreate(5, sizeof(blfm_actuator_command_t));
  configASSERT(xActuatorCmdQueue != NULL);
  
  
  // Create tasks
  xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
              SENSOR_HUB_TASK_PRIORITY, NULL);
  
  xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
              CONTROLLER_TASK_PRIORITY, NULL);
  
  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);
}

void blfm_taskmanager_start(void) { vTaskStartScheduler(); }

// --- Task Implementations ---

static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;

  sensor_data.ultrasonic.distance_mm = 50;

  for (;;) {
    if (blfm_sensor_hub_read(&sensor_data)) {
      xQueueSendToBack(xSensorDataQueue, &sensor_data, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;
  blfm_actuator_command_t command;

  for (;;) {
    if (xQueueReceive(xSensorDataQueue, &sensor_data, pdMS_TO_TICKS(10)) ==
        pdPASS) {
      blfm_controller_process(&sensor_data, &command);
      xQueueSendToBack(xActuatorCmdQueue, &command, 0);
    }      
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_actuator_command_t command;

  for (;;) {
    if (xQueueReceive(xActuatorCmdQueue, &command, pdMS_TO_TICKS(10)) ==
        pdPASS) {
      blfm_actuator_hub_apply(&command);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

/*static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;

  for (;;) {
    sensor_data.ultrasonic.distance_mm = 50;
    vTaskDelay(pdMS_TO_TICKS(50));
  }
  }*/

/*static void vControllerTask(void *pvParameters) {
  (void)pvParameters;
  blfm_actuator_command_t command;

  for (;;) {
    command.led.blink_speed_ms = 20;
    vTaskDelay(pdMS_TO_TICKS(50));
  }
  }*/

/*static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_actuator_command_t command;

  for (;;) {
    command.led.blink_speed_ms = 100;
    command.led.mode = BLFM_LED_MODE_BLINK;
    blfm_actuator_hub_apply(&command);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
*/
