
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

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

/**
 * Initialize the controller state. Call once at startup.
 */
void blfm_controller_init(void);

/**
 * Cycle between MANUAL/AUTO/EMERGENCY modes.
 * Updates the LED and stops motors.
 */
void blfm_controller_cycle_mode(blfm_actuator_command_t *out);

/**
 * Process sensor data and update actuator commands.
 * Typically called periodically in AUTO mode.
 */
void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out);

/**
 * Handle IR remote events in MANUAL mode.
 * Sets motion state and actuator outputs.
 */
void blfm_controller_process_ir_remote(const blfm_ir_remote_event_t *in,
                                       blfm_actuator_command_t *out);

/**
 * Handle joystick direction events in MANUAL mode.
 * Sets motion state and actuator outputs.
 */
void blfm_controller_process_joystick(const blfm_joystick_event_t *evt,
                                      blfm_actuator_command_t *out);

/**
 * Handle joystick button press events.
 * Typically used to cycle modes or trigger actions.
 */
void blfm_controller_process_joystick_click(const blfm_joystick_event_t *event,
                                            blfm_actuator_command_t *command);

/**
 * Handle mode button events with debounce.
 * Typically cycles modes and can trigger servo motions.
 */
void blfm_controller_process_mode_button(const blfm_mode_button_event_t *event,
                                         blfm_actuator_command_t *command);

/**
 * Check if the IR remote has timed out.
 * Currently always returns false.
 */
bool blfm_controller_check_ir_timeout(blfm_actuator_command_t *out);

/**
 * Handle ESP32 command events.
 * Sets motor commands directly from remote input.
 */
void blfm_controller_process_esp32(const blfm_esp32_event_t *event,
                                   blfm_actuator_command_t *out);

/**
 * Handle Big Sound events.
 * Triggers display or alarm patterns when noise is detected.
 */
void blfm_controller_process_bigsound(const blfm_bigsound_event_t *event,
                                      blfm_actuator_command_t *out);


#endif /* BLFM_CONTROLLER_H */
