#ifndef BLFM_IMU_H
#define BLFM_IMU_H

#include <stdint.h>

// IMU sensor data structure (angle in degrees, int16_t)
typedef struct {
    int16_t angle_deg;  // angle in degrees (no float)
    uint8_t valid;      // 1 if data valid, 0 otherwise
} IMUData;

// Initialize the IMU sensor hardware (stub/mock)
void blfm_imu_init(void);

// Read current IMU data
IMUData blfm_imu_read(void);

#endif // BLFM_IMU_H
