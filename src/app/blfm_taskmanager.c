#include "blfm_taskmanager.h"

#include "FreeRTOS.h"
#include "task.h"

#include "blfm_sensor_hub.h"
#include "blfm_actuator_hub.h"

void blfm_taskmanager_start(void) {
    // Start sensor-related tasks
    blfm_sensor_hub_start();

    // Start actuator-related tasks
    blfm_actuator_hub_start();

    // Start the RTOS scheduler
    vTaskStartScheduler();
}
