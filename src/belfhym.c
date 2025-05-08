/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include <unistd.h>
#include <stdio.h>

#include "belfhym.h"
#include "blfm_imu.h"
#include "blfm_pid.h"
#include "blfm_motor.h"
#include "blfm_radio.h"
#include "blfm_debug.h"

void belfhym_greet(void) {
    printf("Welcome to Belfhym — the palm-sized flying machine!\n");
}

void belfhym_init(void) {
    // Initialize IMU
    blfm_imu_init();

    // Initialize PID controllers
    blfm_pid_init();

    // Initialize motors
    blfm_motor_init();

    // Initialize radio (or communication)
    blfm_radio_init();
    
    // Initialize debug features (e.g., LEDs or serial output)
    blfm_debug_init();
}

void belfhym_loop(void) {
    while (1) {
        // Main control loop
        
        // Read IMU data
        blfm_imu_read();
        
        // Apply PID logic
        blfm_pid_update();
        
        // Update motors based on PID output
        blfm_motor_update();
        
        // Handle radio input
        blfm_radio_receive();

        // Debugging (optional)
        blfm_debug_status();
    }
}

int main(void) {
    belfhym_greet();
    
    // Initialize system
    belfhym_init();

    // Enter the main loop
    belfhym_loop();
    
    return 0;
}
