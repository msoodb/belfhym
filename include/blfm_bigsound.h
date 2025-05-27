
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

#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>

void blfm_bigsound_init(void);
void blfm_bigsound_register_controller_task(TaskHandle_t controller_handle);

#endif // BLFM_BIGSOUND_H
