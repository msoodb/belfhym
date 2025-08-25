
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_IR_REMOTE_H
#define BLFM_IR_REMOTE_H

#include "blfm_types.h"
#include "FreeRTOS.h"
#include "queue.h"

void blfm_ir_remote_init(QueueHandle_t event_queue);

#endif /* BLFM_IR_REMOTE_H */
