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
#include "blfm_config.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "blfm_actuator_hub.h"
#include "blfm_controller.h"
#include "blfm_sensor_hub.h"

#if BLFM_ENABLED_MODE_BUTTON
#include "blfm_mode_button.h"
#endif

#if BLFM_ENABLED_IR_REMOTE
#include "blfm_ir_remote.h"
#endif

#if BLFM_ENABLED_ESP32
#include "blfm_esp32.h"
#endif

#if BLFM_ENABLED_BIGSOUND
#include "blfm_bigsound.h"
#endif

// --- Task declarations ---
static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);

// --- Event Handlers ---
static void handle_sensor_data(void);

#if BLFM_ENABLED_BIGSOUND
static void handle_bigsound_event(void);
#endif

#if BLFM_ENABLED_IR_REMOTE
static void handle_ir_remote_event(void);
#endif

#if BLFM_ENABLED_MODE_BUTTON
static void handle_mode_button_event(void);
#endif

#if BLFM_ENABLED_ESP32
static void handle_esp32_event(void);
#endif

// --- Task and queue settings ---
#define SENSOR_HUB_TASK_STACK 256
#define CONTROLLER_TASK_STACK 256
#define ACTUATOR_HUB_TASK_STACK 256

#define SENSOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2

// --- Queues ---
static QueueHandle_t xSensorDataQueue = NULL;
static QueueHandle_t xActuatorCmdQueue = NULL;

#if BLFM_ENABLED_BIGSOUND
static QueueHandle_t xBigSoundQueue = NULL;
#endif

#if BLFM_ENABLED_IR_REMOTE
static QueueHandle_t xIRRemoteQueue = NULL;
#endif

#if BLFM_ENABLED_MODE_BUTTON
static QueueHandle_t xModeButtonQueue = NULL;
#endif

#if BLFM_ENABLED_ESP32
static QueueHandle_t xESP32Queue = NULL;
#endif

static QueueSetHandle_t xControllerQueueSet = NULL;

void blfm_taskmanager_setup(void) {
  // Always create sensor + actuator command queues
  xSensorDataQueue = xQueueCreate(5, sizeof(blfm_sensor_data_t));
  configASSERT(xSensorDataQueue != NULL);

  xActuatorCmdQueue = xQueueCreate(5, sizeof(blfm_actuator_command_t));
  configASSERT(xActuatorCmdQueue != NULL);

  // Optional queues
#if BLFM_ENABLED_BIGSOUND
  xBigSoundQueue = xQueueCreate(5, sizeof(blfm_bigsound_event_t));
  configASSERT(xBigSoundQueue != NULL);
#endif

#if BLFM_ENABLED_IR_REMOTE
  xIRRemoteQueue = xQueueCreate(5, sizeof(blfm_ir_remote_event_t));
  configASSERT(xIRRemoteQueue != NULL);
#endif

#if BLFM_ENABLED_MODE_BUTTON
  xModeButtonQueue = xQueueCreate(5, sizeof(blfm_mode_button_event_t));
  configASSERT(xModeButtonQueue != NULL);
#endif

#if BLFM_ENABLED_ESP32
  xESP32Queue = xQueueCreate(5, sizeof(blfm_esp32_event_t));
  configASSERT(xESP32Queue != NULL);
#endif

  // Queue set
  xControllerQueueSet = xQueueCreateSet(10);
  configASSERT(xControllerQueueSet != NULL);

  xQueueAddToSet(xSensorDataQueue, xControllerQueueSet);

#if BLFM_ENABLED_BIGSOUND
  xQueueAddToSet(xBigSoundQueue, xControllerQueueSet);
#endif
#if BLFM_ENABLED_IR_REMOTE
  xQueueAddToSet(xIRRemoteQueue, xControllerQueueSet);
#endif
#if BLFM_ENABLED_MODE_BUTTON
  xQueueAddToSet(xModeButtonQueue, xControllerQueueSet);
#endif
#if BLFM_ENABLED_ESP32
  xQueueAddToSet(xESP32Queue, xControllerQueueSet);
#endif

  // Init all modules
  blfm_sensor_hub_init();
  blfm_actuator_hub_init();
  blfm_controller_init();

#if BLFM_ENABLED_MODE_BUTTON
  blfm_mode_button_init(xModeButtonQueue);
#endif

#if BLFM_ENABLED_BIGSOUND
  blfm_bigsound_init(xBigSoundQueue);
#endif

#if BLFM_ENABLED_IR_REMOTE
  blfm_ir_remote_init(xIRRemoteQueue);
#endif

#if BLFM_ENABLED_ESP32
  blfm_esp32_init();
#endif

  // Tasks (always run sensor and actuator hub)
  xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
              SENSOR_HUB_TASK_PRIORITY, NULL);

  xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
              CONTROLLER_TASK_PRIORITY, NULL);

  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);
}

void blfm_taskmanager_start(void) { vTaskStartScheduler(); }

// --- Tasks ---
static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;

  for (;;) {
    if (blfm_sensor_hub_read(&sensor_data)) {
      xQueueSendToBack(xSensorDataQueue, &sensor_data, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;

#if BLFM_ENABLED_IR_REMOTE
  blfm_actuator_command_t command;
#endif

#if BLFM_ENABLED_ESP32
  blfm_esp32_event_t esp32_event;
#endif

  for (;;) {
    QueueSetMemberHandle_t activated =
        xQueueSelectFromSet(xControllerQueueSet, pdMS_TO_TICKS(100));

    if (activated == NULL) {
#if BLFM_ENABLED_IR_REMOTE
      if (blfm_controller_check_ir_timeout(&command)) {
        xQueueSendToBack(xActuatorCmdQueue, &command, 0);
      }
#endif
#if BLFM_ENABLED_ESP32
      if (blfm_esp32_get_event(&esp32_event)) {
        xQueueSendToBack(xESP32Queue, &esp32_event, 0);
      }
#endif
      continue;
    }

    if (activated == xSensorDataQueue) {
      handle_sensor_data();
    }
#if BLFM_ENABLED_BIGSOUND
    else if (activated == xBigSoundQueue) {
      handle_bigsound_event();
    }
#endif
#if BLFM_ENABLED_IR_REMOTE
    else if (activated == xIRRemoteQueue) {
      handle_ir_remote_event();
    }
#endif
#if BLFM_ENABLED_MODE_BUTTON
    else if (activated == xModeButtonQueue) {
      handle_mode_button_event();
    }
#endif
#if BLFM_ENABLED_ESP32
    else if (activated == xESP32Queue) {
      handle_esp32_event();
    }
#endif
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

// --- Event Handlers ---
static void handle_sensor_data(void) {
  blfm_sensor_data_t sensor_data;
  blfm_actuator_command_t command;

  if (xQueueReceive(xSensorDataQueue, &sensor_data, 0) == pdPASS) {
    blfm_controller_process(&sensor_data, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

#if BLFM_ENABLED_BIGSOUND
static void handle_bigsound_event(void) {
  blfm_bigsound_event_t event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xBigSoundQueue, &event, 0) == pdPASS) {
    blfm_controller_process_bigsound(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}
#endif

#if BLFM_ENABLED_IR_REMOTE
static void handle_ir_remote_event(void) {
  blfm_ir_remote_event_t event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xIRRemoteQueue, &event, 0) == pdPASS) {
    blfm_controller_process_ir_remote(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}
#endif

#if BLFM_ENABLED_MODE_BUTTON
static void handle_mode_button_event(void) {
  blfm_mode_button_event_t event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xModeButtonQueue, &event, 0) == pdPASS) {
    blfm_controller_process_mode_button(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}
#endif

#if BLFM_ENABLED_ESP32
static void handle_esp32_event(void) {
  blfm_esp32_event_t event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xESP32Queue, &event, 0) == pdPASS) {
    blfm_controller_process_esp32(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}
#endif
