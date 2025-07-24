
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_SERVOMOTOR_H
#define BLFM_SERVOMOTOR_H

#include "blfm_config.h"

#if BLFM_ENABLED_SERVO

#include <stdint.h>
#include "blfm_types.h"

void blfm_servomotor_init(void);
void blfm_servomotor_set_type(blfm_servo_type_t type);
void blfm_servomotor_apply(const blfm_servomotor_command_t *cmd);

#endif /* BLFM_ENABLED_SERVO */

#endif /* BLFM_SERVOMOTOR_H */
