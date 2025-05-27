
#ifndef BLFM_ISR_H
#define BLFM_ISR_H

#include "FreeRTOS.h"
#include "task.h"

// Public function to initialize ISR system
void blfm_isr_init(void);

// Public handle to register the ControllerTask for notifications
void blfm_isr_register_controller_task(TaskHandle_t controller_handle);

#endif // BLFM_ISR_H
