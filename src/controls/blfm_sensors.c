
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_sensors.h"
#include "blfm_ultrasonic.h"
#include "libc_stubs.h" /* IWYU pragma: keep */
#include "FreeRTOS.h" /* IWYU pragma: keep */
#include "task.h"
#include <stdbool.h>

static blfm_sensor_data_t sensor_data;
static bool initialized = false;

void blfm_sensor_hub_init(void) {
    memset(&sensor_data, 0, sizeof(sensor_data));
    blfm_ultrasonic_init();
    initialized = true;
}

bool blfm_sensor_hub_read(blfm_sensor_data_t *out) {
    if (!out || !initialized) {
        return false;
    }
    
    blfm_ultrasonic_read(&sensor_data.ultrasonic);
    sensor_data.system_timestamp_ms = xTaskGetTickCount();
    *out = sensor_data;
    
    return true;
}
