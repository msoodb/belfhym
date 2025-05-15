#include "blfm_imu.h"

// Mock IMU data
static int16_t mock_angles[] = {0, 15, -10, 5, -5};
static uint8_t idx = 0;

void blfm_imu_init(void) {
    // Hardware init code here (mock: do nothing)
}

IMUData blfm_imu_read(void) {
    IMUData data;
    data.angle_deg = mock_angles[idx];
    data.valid = 1;

    idx = (idx + 1) % (sizeof(mock_angles) / sizeof(mock_angles[0]));

    return data;
}
