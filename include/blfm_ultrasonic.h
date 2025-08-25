/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_ULTRASONIC_H
#define BLFM_ULTRASONIC_H

#include "blfm_types.h"
#include <stdbool.h>

void blfm_ultrasonic_init(void);
bool blfm_ultrasonic_read(blfm_ultrasonic_data_t *data);

#endif /* BLFM_ULTRASONIC_H */