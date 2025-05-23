
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_ACTUATOR_HUB_H
#define BLFM_ACTUATOR_HUB_H

#include "blfm_types.h"

void blfm_actuator_hub_init(void);
void blfm_actuator_hub_apply(const blfm_actuator_command_t *cmd);

#endif // BLFM_ACTUATOR_HUB_H

