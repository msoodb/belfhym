
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_ACTUATORS_H
#define BLFM_ACTUATORS_H

#include "blfm_types.h"

void blfm_actuator_hub_init(void);
void blfm_actuator_hub_apply(const blfm_controller_output_t *cmd);
void blfm_actuator_hub_apply_periodic(uint32_t current_time_ms);

#endif /* BLFM_ACTUATORS_H */

