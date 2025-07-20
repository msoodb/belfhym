/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"

#if BLFM_ENABLED_IR_REMOTE

#ifndef BLFM_IR_REMOTE_H
#define BLFM_IR_REMOTE_H

#include "FreeRTOS.h"
#include "blfm_types.h"
#include "queue.h"
#include <stdint.h>

void blfm_ir_remote_init(QueueHandle_t controller_queue);
void ir_exti_handler(void);

#endif // BLFM_IR_REMOTE_H

#endif /* BLFM_ENABLED_IR_REMOTE */
