#ifndef BLFM_ULTRASONIC_H
#define BLFM_ULTRASONIC_H

#include <stdint.h>

// Ultrasonic sensor data structure
typedef struct {
    uint16_t distance_mm;  // distance in millimeters
    uint8_t  valid;        // 1 if reading is valid, 0 otherwise
} UltrasonicData;

// Initialize the ultrasonic sensor hardware (stub/mock for now)
void blfm_ultrasonic_init(void);

// Read current ultrasonic sensor data
UltrasonicData blfm_ultrasonic_read(void);

#endif // BLFM_ULTRASONIC_H
