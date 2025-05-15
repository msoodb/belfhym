#include "blfm_types.h"
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t ultrasonicQueue;
extern QueueHandle_t imuQueue;
extern QueueHandle_t motorQueue;

void PathFindingTask(void *params) {
    UltrasonicData us;
    IMUData imu;
    DriveCommand cmd;

    while (1) {
        if (xQueueReceive(ultrasonicQueue, &us, portMAX_DELAY) == pdPASS &&
            xQueueReceive(imuQueue, &imu, portMAX_DELAY) == pdPASS) {

            if (us.distance_cm < 30) {
                cmd.speed_left = -40;
                cmd.speed_right = -40;
            } else {
                cmd.speed_left = 80;
                cmd.speed_right = 80;
            }

            xQueueSend(motorQueue, &cmd, 0);
        }
    }
}
