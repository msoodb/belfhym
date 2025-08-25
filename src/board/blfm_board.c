
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_board.h"
#include "blfm_gpio.h"
#include "blfm_clock.h"

void blfm_board_init(void) {
  blfm_clock_init();
  blfm_gpio_init();
}

