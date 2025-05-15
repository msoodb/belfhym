
#include "blfm_types.h"
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t motorQueue;

void blfm_motor_log_command(const DriveCommand *cmd) {
    (void)cmd;
}

void MotorTask(void *params) {
    DriveCommand cmd;
    while (1) {
        if (xQueueReceive(motorQueue, &cmd, portMAX_DELAY) == pdPASS) {
            blfm_motor_log_command(&cmd);
        }
    }
}
