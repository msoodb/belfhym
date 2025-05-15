#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "blfm_motor.h"
#include "blfm_pathfinder.h"
#include "blfm_ultrasonic.h"
#include "blfm_imu.h"
#include "blfm_types.h"

QueueHandle_t ultrasonicQueue;
QueueHandle_t imuQueue;
QueueHandle_t motorQueue;

static void UltrasonicTask(void *params) {
    UltrasonicData data;
    while (1) {
        data = read_ultrasonic_mock();
        xQueueSend(ultrasonicQueue, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void IMUTask(void *params) {
    IMUData data;
    while (1) {
        data = read_imu_angle_mock();
        xQueueSend(imuQueue, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void blfm_taskmanager_start(void) {
    ultrasonicQueue = xQueueCreate(5, sizeof(UltrasonicData));
    imuQueue = xQueueCreate(5, sizeof(IMUData));
    motorQueue = xQueueCreate(5, sizeof(DriveCommand));

    xTaskCreate(UltrasonicTask, "Ultrasonic", 128, NULL, 2, NULL);
    xTaskCreate(IMUTask, "IMU", 128, NULL, 2, NULL);
    xTaskCreate(PathFindingTask, "Path", 128, NULL, 2, NULL);
    xTaskCreate(MotorTask, "Motor", 128, NULL, 2, NULL);

    vTaskStartScheduler();
}
