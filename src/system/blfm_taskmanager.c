

#include "blfm_taskmanager.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "blfm_actuators.h"
#include "blfm_controller.h"
#include "blfm_sensors.h"
// #include "blfm_ir_remote.h"
#include "S17.h"
#include "blfm_types.h"
#include "libc_stubs.h" /* IWYU pragma: keep */

#include "blfm_pins.h"
#include "blfm_gpio.h"

static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);
static void vEventProcTask(void *pvParameters);

static void s17_message_handler(const uint8_t *data, uint8_t length);

#define SENSOR_HUB_TASK_STACK 384
#define CONTROLLER_TASK_STACK 768  
#define ACTUATOR_HUB_TASK_STACK 384
#define EVENT_PROCESSING_TASK_STACK 384

#define CONTROLLER_TASK_PRIORITY 5
#define EVENT_PROCESSING_PRIORITY 4
#define ACTUATOR_HUB_TASK_PRIORITY 4
#define SENSOR_HUB_TASK_PRIORITY 3      

static QueueHandle_t xControllerInputQueue = NULL;   
static QueueHandle_t xControllerOutputQueue = NULL;  

static QueueHandle_t xS17EventQueue = NULL;

void blfm_taskmanager_setup(void) {
  xControllerInputQueue = xQueueCreate(8, sizeof(blfm_controller_input_t));
  configASSERT(xControllerInputQueue != NULL);

  xControllerOutputQueue = xQueueCreate(3, sizeof(blfm_controller_output_t));
  configASSERT(xControllerOutputQueue != NULL);

  xS17EventQueue = xQueueCreate(8, sizeof(blfm_nrf24_event_t));
  configASSERT(xS17EventQueue != NULL);

  blfm_sensor_hub_init();
  blfm_actuator_hub_init();
  blfm_controller_init();
  
  if (s17_init() != S17_OK) {
    configASSERT(0);
  }
  
  s17_listen(s17_message_handler);
  
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
  if (s17_start() != S17_OK) {
    configASSERT(0);
  }
  
  vTaskStartScheduler();
  configASSERT(0);
}


static void vEventProcTask(void *pvParameters) {
  (void)pvParameters;
  blfm_nrf24_event_t s17_event;
  blfm_controller_input_t controller_input;

  for (;;) {

    if (xQueueReceive(xS17EventQueue, &s17_event, 0) == pdPASS) {
      controller_input.type = BLFM_INPUT_NRF24;
      controller_input.data.nrf24 = s17_event;
      xQueueSendToBack(xControllerInputQueue, &controller_input, 0);
    }

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_sensor_data_t sensor_data;
  blfm_controller_input_t controller_input;

  for (;;) {
    if (blfm_sensor_hub_read(&sensor_data)) {
      
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
  
  for (;;) {
    if (xQueueReceive(xControllerInputQueue, &controller_input, pdMS_TO_TICKS(10)) == pdPASS) {
      blfm_controller_process_input(&controller_input, &command);
      xQueueSendToBack(xControllerOutputQueue, &command, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;
  blfm_controller_output_t command;

  for (;;) {
    if (xQueueReceive(xControllerOutputQueue, &command, pdMS_TO_TICKS(5)) ==
        pdPASS) {
      blfm_actuator_hub_apply(&command);
    }
    // Removed periodic processing for maximum performance
  }
}

static void s17_message_handler(const uint8_t *data, uint8_t length) {

  if (!data || length < 2) return;

  uint16_t target_node = *(uint16_t*)(data);
  if (target_node != S17_NODE_BELFHYM) {
    return;
  }

  blfm_nrf24_event_t s17_event = {
    .length = length - 2,
    .pipe = 0,
    .rssi = -50
  };

  uint8_t effective_length = length - 2;
  uint8_t copy_len = (effective_length > sizeof(s17_event.data)) ? sizeof(s17_event.data) : effective_length;
  memcpy(s17_event.data, data + 2, copy_len);
  xQueueSendToBack(xS17EventQueue, &s17_event, 0);
}
