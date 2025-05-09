/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include <stdio.h>
#include "belfhym.h"

#include "blfm_imu.h"
#include "blfm_pid.h"
#include "blfm_motor.h"
#include "blfm_radio.h"
#include "blfm_debug.h"

// FreeRTOS headers
#include "FreeRTOS.h"
#include "task.h"


int main(void) {
    belfhym_greet();
    belfhym_init();

    // Create tasks
    // Start scheduler
    vTaskStartScheduler();

    printf("main initialized\n");  

    // Should never reach here
    for (;;) {}
    return 0;
}
