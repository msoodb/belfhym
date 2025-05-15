#include "blfm_ultrasonic.h"

// Mock implementation of ultrasonic sensor reading

void blfm_ultrasonic_init(void) {
    // Hardware initialization code here (mock: do nothing)
}

UltrasonicData blfm_ultrasonic_read(void) {
    static uint16_t mock_distances[] = {100, 150, 200, 250, 300};
    static uint8_t idx = 0;

    UltrasonicData data;
    data.distance_mm = mock_distances[idx];
    data.valid = 1;

    idx = (idx + 1) % (sizeof(mock_distances) / sizeof(mock_distances[0]));

    return data;
}
