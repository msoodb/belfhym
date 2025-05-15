/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "FreeRTOS.h"
#include "task.h"

#include "blfm_taskmanager.h"


int main(void) {
  blfm_taskmanager_start();
  while (1);
}
