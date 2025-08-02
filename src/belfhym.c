
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_board.h"
#include "blfm_taskmanager.h"

int main(void) {

  // Initialize system clocks, peripherals, and low-level hardware
  blfm_board_init();

  // Set up all RTOS tasks and any necessary synchronization primitives
  blfm_taskmanager_setup();

  // Start the FreeRTOS scheduler (does not return)
  blfm_taskmanager_start();

  // Fallback loop: should never reach here if RTOS is running
  while (1) {
    // TODO: Consider low-power mode or error handler
  }
}
