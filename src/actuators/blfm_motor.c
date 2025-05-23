
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_motor.h"

static void blfm_motor_set_speed(uint8_t speed);
static void blfm_motor_set_direction(uint8_t direction);

void blfm_motor_init(void) {
}

void blfm_motor_apply(const blfm_motor_command_t *cmd) {
  if (!cmd) return;
  blfm_motor_set_speed(cmd->speed);
  blfm_motor_set_direction(cmd->direction);
}


static void blfm_motor_set_speed(uint8_t speed) {
  (void) speed;
}

static void blfm_motor_set_direction(uint8_t direction) {
  (void) direction;
}
