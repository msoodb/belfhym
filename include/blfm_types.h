#ifndef BLFM_TYPES_H
#define BLFM_TYPES_H

#include <stdint.h>

typedef struct {
    uint16_t distance_cm;
} UltrasonicData;

typedef struct {
    int16_t angle_deg;
} IMUData;

typedef struct {
    int16_t speed_left;
    int16_t speed_right;
} DriveCommand;

#endif
