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
#include "queue.h"
#include "task.h"

#include "blfm_actuator_hub.h"
#include "blfm_controller.h"
#include "blfm_ir_remote.h"
#include "blfm_mode_button.h"
#include "blfm_sensor_hub.h"

#include "blfm_gpio.h"
#include "blfm_pins.h"

static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);

// Internal handler functions
static void handle_sensor_data(void);
static void handle_bigsound_event(void);
static void handle_ir_remote_event(void);
static void handle_joystick_event(void);
static void handle_mode_button_event(void);

// Task and queue handles
#define SENSOR_HUB_TASK_STACK 256
#define CONTROLLER_TASK_STACK 256
#define ACTUATOR_HUB_TASK_STACK 256

#define SENSOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2

// Queues
static QueueHandle_t xSensorDataQueue = NULL;
static QueueHandle_t xBigSoundQueue = NULL;
static QueueHandle_t xIRRemoteQueue = NULL;
static QueueHandle_t xJoystickQueue = NULL;
static QueueHandle_t xModeButtonQueue = NULL;
static QueueHandle_t xActuatorCmdQueue = NULL;
static QueueSetHandle_t xControllerQueueSet = NULL;

void blfm_taskmanager_setup(void) {
  // Create queues
  xSensorDataQueue = xQueueCreate(5, sizeof(blfm_sensor_data_t));
  configASSERT(xSensorDataQueue != NULL);

  xBigSoundQueue = xQueueCreate(5, sizeof(blfm_bigsound_event_t));
  configASSERT(xBigSoundQueue != NULL);

  xIRRemoteQueue = xQueueCreate(5, sizeof(blfm_ir_remote_event_t));
  configASSERT(xIRRemoteQueue != NULL);

  xJoystickQueue = xQueueCreate(5, sizeof(blfm_joystick_event_t));
  configASSERT(xJoystickQueue != NULL);

  xModeButtonQueue = xQueueCreate(5, sizeof(blfm_mode_button_event_t));
  configASSERT(xModeButtonQueue != NULL);

  xActuatorCmdQueue = xQueueCreate(5, sizeof(blfm_actuator_command_t));
  configASSERT(xActuatorCmdQueue != NULL);

  // Create queue set
  xControllerQueueSet = xQueueCreateSet(10);
  configASSERT(xControllerQueueSet != NULL);

  xQueueAddToSet(xSensorDataQueue, xControllerQueueSet);
  xQueueAddToSet(xBigSoundQueue, xControllerQueueSet);
  xQueueAddToSet(xIRRemoteQueue, xControllerQueueSet);
  xQueueAddToSet(xJoystickQueue, xControllerQueueSet);
  xQueueAddToSet(xModeButtonQueue, xControllerQueueSet);

  // Init subsystems
  blfm_sensor_hub_init();
  blfm_mode_button_init(xModeButtonQueue);
  // blfm_bigsound_init(xBigSoundQueue);
  blfm_ir_remote_init(xIRRemoteQueue);
  blfm_controller_init();
  blfm_actuator_hub_init();

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

  for (;;) {
    if (blfm_sensor_hub_read(&sensor_data)) {
      xQueueSendToBack(xSensorDataQueue, &sensor_data, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;
  blfm_actuator_command_t command;

  for (;;) {
    QueueSetMemberHandle_t activated =
        xQueueSelectFromSet(xControllerQueueSet, pdMS_TO_TICKS(100));

    if (activated == NULL) {
      // No event: just check timeout
      if (blfm_controller_check_ir_timeout(&command)) {
        // Only send if a stop was issued
        xQueueSendToBack(xActuatorCmdQueue, &command, 0);
      }
      continue;
    }
    
    if (activated == xSensorDataQueue) {
      handle_sensor_data();
    } else if (activated == xBigSoundQueue) {
      handle_bigsound_event();
    } else if (activated == xIRRemoteQueue) {
      handle_ir_remote_event();
    } else if (activated == xJoystickQueue) {
      handle_joystick_event();
    } else if (activated == xModeButtonQueue) {
      handle_mode_button_event();
    }
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

static void handle_bigsound_event(void) {
  blfm_bigsound_event_t bigsound_event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xBigSoundQueue, &bigsound_event, 0) == pdPASS) {
    blfm_controller_process_bigsound(&bigsound_event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

static void handle_ir_remote_event(void) {
  blfm_ir_remote_event_t ir_event;
  blfm_actuator_command_t command;
  
  if (xQueueReceive(xIRRemoteQueue, &ir_event, 0) == pdPASS) {
    blfm_controller_process_ir_remote(&ir_event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

static void handle_joystick_event(void) {
  blfm_joystick_event_t joystick_event;
  blfm_actuator_command_t command;

  if (xQueueReceive(xJoystickQueue, &joystick_event, 0) == pdPASS) {
    // Check if it's a click event
    if (joystick_event.event_type == BLFM_JOYSTICK_EVENT_PRESSED) {
      // Click detected
      blfm_controller_process_joystick_click(&joystick_event, &command);
      xQueueSendToBack(xActuatorCmdQueue, &command, 0);
    } else if (joystick_event.event_type == BLFM_JOYSTICK_EVENT_RELEASED) {
      // Release detected (you can process if needed)
      // Currently ignoring release
    } else {
      // Movement (no click/release), use normal joystick process
      blfm_controller_process_joystick(&joystick_event, &command);
      xQueueSendToBack(xActuatorCmdQueue, &command, 0);
    }
  }
}

static void handle_mode_button_event(void) {
  blfm_mode_button_event_t mode_event;

  if (xQueueReceive(xModeButtonQueue, &mode_event, 0) == pdPASS) {
    blfm_actuator_command_t command;
    blfm_controller_process_mode_button(&mode_event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}
