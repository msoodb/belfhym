
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_taskmanager.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "blfm_actuators.h"
#include "blfm_controller.h"
#include "blfm_sensors.h"
#include "blfm_button.h"
#include "blfm_led.h"
#include "S17.h"
#include "S17_config.h"
#include "blfm_types.h"
#include <string.h>


/* Task declarations */
static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);
static void vEventProcTask(void *pvParameters);

/* S17 callback functions for modern API */
static void s17_message_handler(s17_msg_type_t type, uint16_t sender_id, const uint8_t *data, uint8_t length);
static void s17_telemetry_handler(uint16_t sender_id, const s17_msg_status_t *status);
static void s17_error_handler(s17_result_t error_code, uint32_t info);

/* Task and queue settings - Reduced for memory constraints */
#define SENSOR_HUB_TASK_STACK 384
#define CONTROLLER_TASK_STACK 768  
#define ACTUATOR_HUB_TASK_STACK 384
#define EVENT_PROCESSING_TASK_STACK 384

/* Task priorities - Controller highest for real-time control */
#define CONTROLLER_TASK_PRIORITY 4      /* Highest - critical control loop */
#define EVENT_PROCESSING_PRIORITY 3     /* High - event processing */
#define ACTUATOR_HUB_TASK_PRIORITY 3    /* High - actuator response */
#define SENSOR_HUB_TASK_PRIORITY 2      /* Medium - sensor reading */

/* Queues */
static QueueHandle_t xControllerInputQueue = NULL;   /* Unified input queue */
static QueueHandle_t xControllerOutputQueue = NULL;  /* Controller output/actuator commands */

/* Device interface queues - provide clean abstraction for device drivers */
static QueueHandle_t xButtonEventQueue = NULL;
static QueueHandle_t xS17EventQueue = NULL;  /* S17 communication events */

void blfm_taskmanager_setup(void) {
  /* Create all queues */
  xControllerInputQueue = xQueueCreate(20, sizeof(blfm_controller_input_t));
  configASSERT(xControllerInputQueue != NULL);

  xControllerOutputQueue = xQueueCreate(5, sizeof(blfm_controller_output_t));
  configASSERT(xControllerOutputQueue != NULL);
  
  /* Device interface queues - clean abstraction between devices and control system */
  xButtonEventQueue = xQueueCreate(10, sizeof(blfm_button_event_t));
  configASSERT(xButtonEventQueue != NULL);
  
  
  xS17EventQueue = xQueueCreate(10, sizeof(blfm_nrf24_event_t));  /* S17 uses same event structure */
  configASSERT(xS17EventQueue != NULL);

  /* Init all modules */
  blfm_sensor_hub_init();
  blfm_actuator_hub_init();
  blfm_controller_init();
  blfm_button_init(xButtonEventQueue);
  
  /* Initialize S17 communication module */
  static uint8_t mission_nmk[] = S17_MISSION_NMK;
  s17_config_t s17_config = {
    .node_id = S17_DEVICE_ID,
    .role = S17_IS_NOC() ? S17_TIMING_MASTER : S17_TIMING_SLAVE
  };
  memcpy(s17_config.network_key, mission_nmk, sizeof(s17_config.network_key));
  
  if (s17_init(&s17_config) != S17_OK) {
    configASSERT(0);
  }
  
  /* Register S17 callbacks */
  s17_set_message_callback(s17_message_handler);
  s17_set_telemetry_callback(s17_telemetry_handler);
  s17_set_error_callback(s17_error_handler);

  /* Create all tasks */
  xTaskCreate(vEventProcTask, "EventProc", EVENT_PROCESSING_TASK_STACK, NULL,
              EVENT_PROCESSING_PRIORITY, NULL);

  xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
              SENSOR_HUB_TASK_PRIORITY, NULL);

  xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
              CONTROLLER_TASK_PRIORITY, NULL);

  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);


  
}

void blfm_taskmanager_start(void) {
  /* Start FreeRTOS scheduler */
  vTaskStartScheduler();
  
  /* Should never reach here */
  configASSERT(0);
}

/* Task Implementations */
static void vEventProcTask(void *pvParameters) {
  (void)pvParameters;
  blfm_button_event_t button_event;
  blfm_nrf24_event_t s17_event;  /* S17 uses same event structure */
  blfm_controller_input_t controller_input;

  /* Event processing task - forwards events to unified controller input queue */
  for (;;) {
    /* Forward button events to controller */
    if (xQueueReceive(xButtonEventQueue, &button_event, 0) == pdPASS) {
      controller_input.type = BLFM_INPUT_BUTTON;
      controller_input.data.button = button_event;
      xQueueSendToBack(xControllerInputQueue, &controller_input, 0);
    }
    
    
    /* Forward S17 communication events to controller */
    if (xQueueReceive(xS17EventQueue, &s17_event, 0) == pdPASS) {
      controller_input.type = BLFM_INPUT_NRF24;  /* Keep same type for compatibility */
      controller_input.data.nrf24 = s17_event;
      xQueueSendToBack(xControllerInputQueue, &controller_input, 0);
    }
    
    /* Small delay to prevent CPU hogging */
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;
  blfm_controller_input_t controller_input;

  /* Sensor reading task - forwards sensor data to unified controller input queue */
  for (;;) {
    if (blfm_sensor_hub_read(&sensor_data)) {
      /* Forward sensor data to controller */
      controller_input.type = BLFM_INPUT_SENSOR;
      controller_input.data.sensor = sensor_data;
      xQueueSendToBack(xControllerInputQueue, &controller_input, 0);
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;
  blfm_controller_input_t controller_input;
  blfm_controller_output_t command;
  

  /* Unified controller task - processes ALL inputs through single entry point */
  for (;;) {
    /* Process unified input (sensors, events, etc.) */
    if (xQueueReceive(xControllerInputQueue, &controller_input, pdMS_TO_TICKS(20)) == pdPASS) {
      /* Single entry point for ALL controller decisions */
      blfm_controller_process_input(&controller_input, &command);
      xQueueSendToBack(xControllerOutputQueue, &command, 0);
    }
    
    /* Small delay to yield CPU */
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_controller_output_t command;
  TickType_t xLastPeriodicTime = xTaskGetTickCount();

  for (;;) {
    /* Process commands from controller */
    if (xQueueReceive(xControllerOutputQueue, &command, pdMS_TO_TICKS(10)) ==
        pdPASS) {
      blfm_actuator_hub_apply(&command);
    }
    
    /* Run periodic actuator tasks every 10ms */
    TickType_t xCurrentTime = xTaskGetTickCount();
    if ((xCurrentTime - xLastPeriodicTime) >= pdMS_TO_TICKS(10)) {
      blfm_actuator_hub_apply_periodic(xCurrentTime);
      xLastPeriodicTime = xCurrentTime;
    }
  }
}

/* S17 callback implementations for modern API */
static void s17_message_handler(s17_msg_type_t type, uint16_t sender_id, const uint8_t *data, uint8_t length) {
  if (!data || length == 0) return;

  /* Create NRF24 event from received S17 data */
  blfm_nrf24_event_t nrf24_event;
  
  /* S17 callback has already filtered and parsed the message */
  /* sender_id contains the sender, data contains clean payload */
  nrf24_event.length = (length > S17_MAX_PAYLOAD_SIZE) ? S17_MAX_PAYLOAD_SIZE : length;
  for (uint8_t i = 0; i < nrf24_event.length; i++) {
    nrf24_event.data[i] = data[i];
  }
  nrf24_event.pipe = 0;      /* Default pipe */
  nrf24_event.rssi = -50;    /* Default RSSI value */
  
  /* Send event to S17 event queue for processing */
  if (xS17EventQueue != NULL) {
    xQueueSendToBack(xS17EventQueue, &nrf24_event, 0);  /* Don't block */
  }
  
  /* Suppress unused parameter warnings */
  (void)type;
  (void)sender_id;
}

static void s17_telemetry_handler(uint16_t sender_id, const s17_msg_status_t *status) {
  /* Handle S17 telemetry data if needed */
  (void)sender_id;
  (void)status;
}

static void s17_error_handler(s17_result_t error_code, uint32_t info) {
  /* Handle S17 errors - could log or take corrective action */
  (void)error_code;
  (void)info;
}
