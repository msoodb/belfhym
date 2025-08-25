
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_board.h"
#include "blfm_taskmanager.h"

/* Main entry point for robot system */
int main(void) {
  /* Initialize hardware: clock (72MHz), GPIO, PWM */
  blfm_board_init();
  
  /* Create all FreeRTOS tasks for robot operation */
  blfm_taskmanager_setup();
  
  /* Start FreeRTOS scheduler - never returns */
  blfm_taskmanager_start();
  
  /* Should never reach here - scheduler runs forever */
  while (1) {} 
}
