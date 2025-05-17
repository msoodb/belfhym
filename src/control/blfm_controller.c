
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */
#include "FreeRTOS.h"
#include "task.h"

#include "blfm_actuator_hub.h"
#include "blfm_controller.h"
#include "blfm_sensor_hub.h"

void blfm_controller_init(void) {
  // Nothing needed for now
}

void blfm_controller_run(void) {
  blfm_sensor_data_t sensor_data;

  // Try to get the latest sensor data from sensor hub
  if (blfm_sensor_hub_get_data(&sensor_data)) {
    blfm_motor_command_t motor_cmd = {0};

    // Basic decision logic: stop if obstacle detected closer than 200 mm
    if (sensor_data.ultrasonic.distance_mm < 200) {
      motor_cmd.speed = 0;
      motor_cmd.direction = 0; // stop
    } else {
      motor_cmd.speed = 128;
      motor_cmd.direction = 1; // forward
    }

    // Send command to actuator hub
    // blfm_actuator_hub_send_motor_command(&motor_cmd);
  }
}
