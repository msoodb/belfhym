/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#ifndef BLFM_SERVOMOTOR_H
#define BLFM_SERVOMOTOR_H

#include "blfm_types.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Servo status structure for monitoring
 * Uses fixed-point math: angle_x10 represents angle * 10 (decidegrees)
 * Example: angle_x10 = 450 means 45.0 degrees
 */
typedef struct {
    int16_t current_angle_x10;  /* Current servo angle * 10 */
    int16_t target_angle_x10;   /* Target servo angle * 10 */
    uint16_t current_pulse_us;  /* Current pulse width in microseconds */
    bool enabled;               /* Servo channel enabled */
    blfm_servo_type_t type;     /* Servo operation type */
} blfm_servo_status_t;

/**
 * Initialize servo control system
 * - Uses TIM1 for dedicated servo PWM (50Hz)
 * - Configures PA8-PA11 as servo outputs (TIM1_CH1-4)
 * - All servos start at center position (1500us)
 * - Servos start disabled for safety
 * - Resolves timer conflicts with motor module
 */
void blfm_servomotor_init(void);

/**
 * Enable servo channel for operation
 * @param channel Servo channel (0-3)
 */
void blfm_servomotor_enable(uint8_t channel);

/**
 * Disable servo channel (stops PWM output)
 * @param channel Servo channel (0-3)
 */
void blfm_servomotor_disable(uint8_t channel);

/**
 * Set servo operation type
 * @param channel Servo channel (0-3)
 * @param type Servo type (MANUAL, PROPORTIONAL, STATIC, etc.)
 */
void blfm_servomotor_set_type(uint8_t channel, blfm_servo_type_t type);

/**
 * Set servo direction reversal
 * @param channel Servo channel (0-3)
 * @param reverse true to reverse direction, false for normal
 */
void blfm_servomotor_set_reverse(uint8_t channel, bool reverse);

/**
 * Set servo trim adjustment
 * @param channel Servo channel (0-3)
 * @param trim_us Trim adjustment in microseconds (-200 to +200)
 */
void blfm_servomotor_set_trim(uint8_t channel, int16_t trim_us);

/**
 * Enable/disable smooth servo movement
 * @param channel Servo channel (0-3)
 * @param smooth true for smooth movement, false for immediate
 */
void blfm_servomotor_set_smooth(uint8_t channel, bool smooth);

/**
 * Update system time for smooth movement calculations
 * Should be called from main loop with current system time
 * @param current_time_ms Current system time in milliseconds
 */
void blfm_servomotor_update_time(uint32_t current_time_ms);

/**
 * Apply servo command based on servo type
 * @param channel Servo channel (0-3)
 * @param cmd Servo command structure
 * 
 * Features:
 * - Type-based command interpretation
 * - Input validation and clamping
 * - Direction reversal support
 * - Smooth movement integration
 * - Fixed-point math for performance
 */
void blfm_servomotor_apply(uint8_t channel, const blfm_servomotor_command_t *cmd);

/**
 * Update servo position (for smooth movement)
 * Called automatically by apply, but can be called manually
 * @param channel Servo channel (0-3)
 */
void blfm_servomotor_update(uint8_t channel);

/**
 * Set servo to specific angle using fixed-point math
 * @param channel Servo channel (0-3)
 * @param angle_x10 Target angle * 10 (-900 to +900, representing -90.0° to +90.0°)
 * 
 * Examples: 
 * - angle_x10 = 0 → 0.0° (center)
 * - angle_x10 = 450 → 45.0° 
 * - angle_x10 = -900 → -90.0° (full left)
 */
void blfm_servomotor_set_angle(uint8_t channel, int16_t angle_x10);

/**
 * Set servo pulse width directly
 * @param channel Servo channel (0-3)
 * @param pulse_us Pulse width in microseconds (1000-2000)
 * 
 * Note: Includes safety validation - invalid pulses are rejected
 */
void blfm_servomotor_set_pulse_us(uint8_t channel, uint16_t pulse_us);

/**
 * Get current servo status
 * @param channel Servo channel (0-3)
 * @param status Pointer to status structure to fill
 */
void blfm_servomotor_get_status(uint8_t channel, blfm_servo_status_t *status);

/**
 * Emergency stop all servos
 * Immediately disables all servo outputs
 */
void blfm_servomotor_emergency_stop(void);

#endif /* BLFM_SERVOMOTOR_H */