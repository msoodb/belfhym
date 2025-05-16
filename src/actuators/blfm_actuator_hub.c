
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_actuator_hub.h"
#include "blfm_alarm.h"
#include "blfm_motor.h"

void blfm_actuator_hub_init(void) {
  blfm_motor_init();
  blfm_alarm_init();
}

void blfm_actuator_hub_start(void) {
  // Start actuator-related operations (mock)
}
