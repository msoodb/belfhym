#ifndef BLFM_BIGSOUND_H
#define BLFM_BIGSOUND_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

#include "blfm_types.h"

void blfm_bigsound_init(QueueHandle_t controller_queue);
void blfm_bigsound_isr_handler(void);

#endif // BLFM_BIGSOUND_H
