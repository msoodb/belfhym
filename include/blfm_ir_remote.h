/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_IR_REMOTE_H
#define BLFM_IR_REMOTE_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "blfm_types.h"

void blfm_ir_remote_init(QueueHandle_t controller_queue);
void blfm_ir_task(void *params);
#endif // BLFM_IR_REMOTE_H
