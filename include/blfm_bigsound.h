
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_BIGSOUND_H
#define BLFM_BIGSOUND_H

#include <stdint.h>
#include <stdbool.h>

#include "blfm_types.h"

void blfm_bigsound_init(void);
bool blfm_bigsound_read(blfm_bigsound_data_t *data);

#endif // BLFM_BIGSOUND_H
