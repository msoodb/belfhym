
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_STEPMOTOR_H
#define BLFM_STEPMOTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "blfm_types.h"

// Enum to label each stepper motor
typedef enum {
  BLFM_STEPMOTOR_NECK = 0,
  BLFM_STEPMOTOR_ELBOW,
  BLFM_STEPMOTOR_WRIST,
  BLFM_STEPMOTOR_COUNT // always last
} blfm_stepmotor_id_t;


// === API ===
void blfm_stepmotor_init(void);
void blfm_stepmotor_apply(blfm_stepmotor_id_t id, const blfm_stepmotor_command_t *cmd);
void blfm_stepmotor_apply_all(const blfm_stepmotor_command_t cmds[BLFM_STEPMOTOR_COUNT]);

#endif // BLFM_STEPMOTOR_H
