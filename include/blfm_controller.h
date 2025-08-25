#ifndef BLFM_CONTROLLER_H
#define BLFM_CONTROLLER_H

#include "blfm_types.h"

void blfm_controller_init(void);

/* Unified controller process function */
void blfm_controller_process_input(const blfm_controller_input_t *input,
                                   blfm_controller_output_t *command);

#endif