
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"

#if BLFM_ENABLED_RADIO

#ifndef BLFM_RADIO_H
#define BLFM_RADIO_H

#include "blfm_types.h"

void blfm_radio_init(void);
void blfm_radio_apply(const blfm_radio_command_t *cmd);

#endif // BLFM_RADIO_H

#endif /* BLFM_ENABLED_RADIO */

