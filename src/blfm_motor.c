/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_motor.h"
#include <stdio.h>


void blfm_motor_init(void) {
    printf("MOTOR initialized\n");  
}

void blfm_motor_set_throttle(void /*motor_id, pwm*/){
  printf("MOTOR SET THROTTLE\n");
}

void blfm_motor_update(void /*pid_outputs*/){
  printf("MOTOR UPDATE\n");
}
