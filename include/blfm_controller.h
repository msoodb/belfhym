
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_CONTROLLER_H
#define BLFM_CONTROLLER_H

#include "blfm_types.h"

void blfm_controller_init(void);
void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out);
void blfm_controller_process_bigsound(const blfm_bigsound_event_t *event,
                                      blfm_actuator_command_t *out);
void blfm_controller_process_ir_remote(const blfm_ir_remote_event_t *in,
                                       blfm_actuator_command_t *out);
void blfm_controller_process_joystick(const blfm_joystick_event_t *in,
                                      blfm_actuator_command_t *out) ;
void blfm_controller_process_joystick_click(const blfm_joystick_event_t *event,
                                            blfm_actuator_command_t *command);
void blfm_controller_process_mode_button(const blfm_mode_button_event_t *event,
                                         blfm_actuator_command_t *command);
bool blfm_controller_check_ir_timeout(blfm_actuator_command_t *out);

#endif // BLFM_CONTROLLER_H

