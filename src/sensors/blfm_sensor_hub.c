
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_sensor_hub.h"
#include "blfm_ultrasonic.h"
#include "blfm_imu.h"

void blfm_sensor_hub_init(void) {
    // Initialize all individual sensors
    blfm_ultrasonic_init();
    blfm_imu_init();
}

void blfm_sensor_hub_start(void) {
    // Placeholder for starting sensor operations if needed
    // For example: start timers or enable interrupts for sensors
}

// Optional: if you want a poll function to centralize sensor polling
void blfm_sensor_hub_poll(void) {
    // Example: just read sensor data here if polling-based
    // uint16_t distance = blfm_ultrasonic_get_distance();
    // blfm_imu_data_t imu_data;
    // blfm_imu_get_data(&imu_data);
    
    // For now, just placeholders without doing anything
}
