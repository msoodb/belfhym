
#include "blfm_imu.h"

static int16_t imu_mock_data[] = {0, 15, -10, 5, -5};
static uint8_t imu_index = 0;

IMUData read_imu_angle_mock(void) {
    IMUData data;
    data.angle_deg = imu_mock_data[imu_index];
    imu_index = (imu_index + 1) % (sizeof(imu_mock_data) / sizeof(imu_mock_data[0]));
    return data;
}
