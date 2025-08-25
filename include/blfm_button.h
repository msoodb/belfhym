
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_BUTTON_H
#define BLFM_BUTTON_H

#include "blfm_types.h"
#include "FreeRTOS.h"
#include "queue.h"

void blfm_button_init(QueueHandle_t event_queue);

#endif /* BLFM_BUTTON_H */
