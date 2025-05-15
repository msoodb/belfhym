#include "blfm_ultrasonic.h"

static uint16_t mock_values[] = {50, 45, 40, 25, 60, 30};
static uint8_t index = 0;

UltrasonicData read_ultrasonic_mock(void) {
    UltrasonicData data;
    data.distance_cm = mock_values[index];
    index = (index + 1) % (sizeof(mock_values) / sizeof(mock_values[0]));
    return data;
}
