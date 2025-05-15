#include "FreeRTOS.h"
#include "task.h"

#include "blfm_sensor_hub.h"
#include "blfm_ultrasonic.h"
#include "blfm_imu.h"

// Static storage of latest sensor data
static UltrasonicData ultrasonic_data;
static IMUData imu_data;

void blfm_sensor_hub_init(void) {
    blfm_ultrasonic_init();
    blfm_imu_init();
    // Initialize other sensors here
}

void blfm_sensor_hub_update(void) {
    ultrasonic_data = blfm_ultrasonic_read();
    imu_data = blfm_imu_read();
    // Update other sensors here
}

UltrasonicData blfm_sensor_hub_get_ultrasonic(void) {
    return ultrasonic_data;
}

IMUData blfm_sensor_hub_get_imu(void) { return imu_data; }

void blfm_sensor_hub_start(void) {
  //xTaskCreate(blfm_ultrasonic_task, "Ultrasonic", 128, NULL, 2, NULL);
  //xTaskCreate(blfm_imu_task, "IMU", 128, NULL, 2, NULL);
}
