
/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_CONFIG_H
#define BLFM_CONFIG_H

/* === Sensors presence flags === */
#define BLFM_ENABLED_ULTRASONIC 1
#define BLFM_ENABLED_POTENTIOMETER 0
#define BLFM_ENABLED_TEMPERATURE 0
#define BLFM_ENABLED_JOYSTICK 0

/* === Actuators presence flags === */
#define BLFM_ENABLED_LED 1
#define BLFM_ENABLED_MOTOR 1
#define BLFM_ENABLED_DISPLAY 0
#define BLFM_ENABLED_ALARM 0
#define BLFM_ENABLED_SERVO 1
#define BLFM_ENABLED_RADIO 0
#define BLFM_ENABLED_OLED 1

/* === input/event features === */
#define BLFM_ENABLED_BIGSOUND 0
#define BLFM_ENABLED_IR_REMOTE 1
#define BLFM_ENABLED_MODE_BUTTON 1
#define BLFM_ENABLED_ESP32 0

#endif /* BLFM_CONFIG_H */
