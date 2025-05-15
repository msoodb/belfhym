#ifndef BLFM_SENSOR_HUB_H
#define BLFM_SENSOR_HUB_H

#include <stdint.h>
#include "blfm_types.h"

// Initialize the sensor hub and its submodules (sensors)
void blfm_sensor_hub_init(void);

// Update all sensors data (called periodically)
void blfm_sensor_hub_update(void);

// Get the latest ultrasonic sensor data
UltrasonicData blfm_sensor_hub_get_ultrasonic(void);

// Get the latest IMU data
IMUData blfm_sensor_hub_get_imu(void);

// Add other sensor getters as needed...

void blfm_sensor_hub_start(void);

#endif // BLFM_SENSOR_HUB_H
