/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_TIMER_H
#define BLFM_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Timer peripheral enumeration
 */
typedef enum {
  BLFM_TIMER1 = 0,
  BLFM_TIMER2,
  BLFM_TIMER3,
  BLFM_TIMER4,
  BLFM_TIMER_COUNT
} blfm_timer_peripheral_t;

/**
 * @brief Timer channel enumeration
 */
typedef enum {
  BLFM_TIMER_CHANNEL_1 = 0,
  BLFM_TIMER_CHANNEL_2,
  BLFM_TIMER_CHANNEL_3,
  BLFM_TIMER_CHANNEL_4,
  BLFM_TIMER_CHANNEL_COUNT
} blfm_timer_channel_t;

/**
 * @brief Timer mode enumeration
 */
typedef enum {
  BLFM_TIMER_MODE_BASIC = 0,        // Basic timer mode
  BLFM_TIMER_MODE_PWM,              // PWM generation
  BLFM_TIMER_MODE_INPUT_CAPTURE,    // Input capture
  BLFM_TIMER_MODE_OUTPUT_COMPARE,   // Output compare
  BLFM_TIMER_MODE_ENCODER,          // Encoder mode
  BLFM_TIMER_MODE_ONE_PULSE         // One pulse mode
} blfm_timer_mode_t;

/**
 * @brief PWM mode enumeration
 */
typedef enum {
  BLFM_PWM_MODE_1 = 0,              // PWM mode 1
  BLFM_PWM_MODE_2                   // PWM mode 2
} blfm_pwm_mode_t;

/**
 * @brief Input capture polarity enumeration
 */
typedef enum {
  BLFM_IC_POLARITY_RISING = 0,      // Rising edge
  BLFM_IC_POLARITY_FALLING,         // Falling edge
  BLFM_IC_POLARITY_BOTH             // Both edges
} blfm_ic_polarity_t;

/**
 * @brief Timer configuration structure
 */
typedef struct {
  uint32_t prescaler;               // Timer prescaler (0-65535)
  uint32_t period;                  // Timer period (0-65535)
  uint32_t clock_division;          // Clock division factor
  bool counter_direction_up;        // true = up counting, false = down counting
  bool auto_reload_preload;         // Auto-reload preload enable
} blfm_timer_config_t;

/**
 * @brief PWM channel configuration
 */
typedef struct {
  blfm_timer_channel_t channel;
  blfm_pwm_mode_t mode;
  uint32_t pulse_width;             // Pulse width (0-period)
  bool output_enable;               // Output enable
  bool output_polarity_high;        // true = active high, false = active low
} blfm_pwm_channel_config_t;

/**
 * @brief Input capture channel configuration
 */
typedef struct {
  blfm_timer_channel_t channel;
  blfm_ic_polarity_t polarity;
  uint8_t prescaler;                // Input capture prescaler (1, 2, 4, 8)
  uint8_t filter;                   // Input capture filter (0-15)
} blfm_ic_channel_config_t;

/**
 * @brief Output compare channel configuration
 */
typedef struct {
  blfm_timer_channel_t channel;
  uint32_t compare_value;           // Compare value (0-65535)
  bool output_enable;               // Output enable
  bool output_polarity_high;        // true = active high, false = active low
  bool fast_mode;                   // Fast mode enable
} blfm_oc_channel_config_t;

/**
 * @brief Timer error codes
 */
typedef enum {
  BLFM_TIMER_OK = 0,
  BLFM_TIMER_ERR_INVALID_PERIPHERAL,
  BLFM_TIMER_ERR_INVALID_CHANNEL,
  BLFM_TIMER_ERR_INVALID_CONFIG,
  BLFM_TIMER_ERR_NOT_INITIALIZED,
  BLFM_TIMER_ERR_NULL_PTR,
  BLFM_TIMER_ERR_BUSY
} blfm_timer_error_t;

/**
 * @brief Timer callback function types
 */
typedef void (*blfm_timer_callback_t)(blfm_timer_peripheral_t timer);
typedef void (*blfm_timer_ic_callback_t)(blfm_timer_peripheral_t timer, blfm_timer_channel_t channel, uint32_t value);
typedef void (*blfm_timer_oc_callback_t)(blfm_timer_peripheral_t timer, blfm_timer_channel_t channel);

/**
 * @brief Timer callbacks structure
 */
typedef struct {
  blfm_timer_callback_t update_callback;
  blfm_timer_ic_callback_t ic_callback;
  blfm_timer_oc_callback_t oc_callback;
} blfm_timer_callbacks_t;

/**
 * @brief Initialize timer peripheral
 * @param peripheral Timer peripheral to initialize
 * @param config Configuration structure (can be NULL for default)
 * @param callbacks Callback functions (can be NULL)
 * @return Error code
 */
blfm_timer_error_t blfm_timer_init(blfm_timer_peripheral_t peripheral, 
                                   const blfm_timer_config_t *config,
                                   const blfm_timer_callbacks_t *callbacks);

/**
 * @brief Deinitialize timer peripheral
 * @param peripheral Timer peripheral to deinitialize
 * @return Error code
 */
blfm_timer_error_t blfm_timer_deinit(blfm_timer_peripheral_t peripheral);

/**
 * @brief Start timer
 * @param peripheral Timer peripheral
 * @return Error code
 */
blfm_timer_error_t blfm_timer_start(blfm_timer_peripheral_t peripheral);

/**
 * @brief Stop timer
 * @param peripheral Timer peripheral
 * @return Error code
 */
blfm_timer_error_t blfm_timer_stop(blfm_timer_peripheral_t peripheral);

/**
 * @brief Configure PWM channel
 * @param peripheral Timer peripheral
 * @param config PWM channel configuration
 * @return Error code
 */
blfm_timer_error_t blfm_timer_configure_pwm(blfm_timer_peripheral_t peripheral, 
                                            const blfm_pwm_channel_config_t *config);

/**
 * @brief Start PWM output
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_start_pwm(blfm_timer_peripheral_t peripheral, 
                                        blfm_timer_channel_t channel);

/**
 * @brief Stop PWM output
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_stop_pwm(blfm_timer_peripheral_t peripheral, 
                                       blfm_timer_channel_t channel);

/**
 * @brief Set PWM pulse width
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @param pulse_width Pulse width value
 * @return Error code
 */
blfm_timer_error_t blfm_timer_set_pwm_pulse_width(blfm_timer_peripheral_t peripheral, 
                                                  blfm_timer_channel_t channel, 
                                                  uint32_t pulse_width);

/**
 * @brief Configure input capture channel
 * @param peripheral Timer peripheral
 * @param config Input capture channel configuration
 * @return Error code
 */
blfm_timer_error_t blfm_timer_configure_input_capture(blfm_timer_peripheral_t peripheral, 
                                                      const blfm_ic_channel_config_t *config);

/**
 * @brief Start input capture
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_start_input_capture(blfm_timer_peripheral_t peripheral, 
                                                  blfm_timer_channel_t channel);

/**
 * @brief Stop input capture
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_stop_input_capture(blfm_timer_peripheral_t peripheral, 
                                                 blfm_timer_channel_t channel);

/**
 * @brief Get input capture value
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Capture value
 */
uint32_t blfm_timer_get_capture_value(blfm_timer_peripheral_t peripheral, 
                                      blfm_timer_channel_t channel);

/**
 * @brief Configure output compare channel
 * @param peripheral Timer peripheral
 * @param config Output compare channel configuration
 * @return Error code
 */
blfm_timer_error_t blfm_timer_configure_output_compare(blfm_timer_peripheral_t peripheral, 
                                                       const blfm_oc_channel_config_t *config);

/**
 * @brief Start output compare
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_start_output_compare(blfm_timer_peripheral_t peripheral, 
                                                   blfm_timer_channel_t channel);

/**
 * @brief Stop output compare
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @return Error code
 */
blfm_timer_error_t blfm_timer_stop_output_compare(blfm_timer_peripheral_t peripheral, 
                                                  blfm_timer_channel_t channel);

/**
 * @brief Set output compare value
 * @param peripheral Timer peripheral
 * @param channel Timer channel
 * @param compare_value Compare value
 * @return Error code
 */
blfm_timer_error_t blfm_timer_set_compare_value(blfm_timer_peripheral_t peripheral, 
                                                blfm_timer_channel_t channel, 
                                                uint32_t compare_value);

/**
 * @brief Get timer counter value
 * @param peripheral Timer peripheral
 * @return Counter value
 */
uint32_t blfm_timer_get_counter(blfm_timer_peripheral_t peripheral);

/**
 * @brief Set timer counter value
 * @param peripheral Timer peripheral
 * @param counter_value Counter value
 * @return Error code
 */
blfm_timer_error_t blfm_timer_set_counter(blfm_timer_peripheral_t peripheral, 
                                          uint32_t counter_value);

/**
 * @brief Generate update event
 * @param peripheral Timer peripheral
 * @return Error code
 */
blfm_timer_error_t blfm_timer_generate_update(blfm_timer_peripheral_t peripheral);

/**
 * @brief Enable timer interrupt
 * @param peripheral Timer peripheral
 * @param interrupt_type Interrupt type bitmask
 * @return Error code
 */
blfm_timer_error_t blfm_timer_enable_interrupt(blfm_timer_peripheral_t peripheral, 
                                               uint32_t interrupt_type);

/**
 * @brief Disable timer interrupt
 * @param peripheral Timer peripheral
 * @param interrupt_type Interrupt type bitmask
 * @return Error code
 */
blfm_timer_error_t blfm_timer_disable_interrupt(blfm_timer_peripheral_t peripheral, 
                                                uint32_t interrupt_type);

/**
 * @brief Timer interrupt handler
 * @param peripheral Timer peripheral
 */
void blfm_timer_interrupt_handler(blfm_timer_peripheral_t peripheral);

/**
 * @brief Calculate timer settings for frequency
 * @param target_frequency_hz Target frequency in Hz
 * @param timer_clock_hz Timer clock frequency in Hz
 * @param prescaler Pointer to store calculated prescaler
 * @param period Pointer to store calculated period
 * @return Error code
 */
blfm_timer_error_t blfm_timer_calculate_frequency(uint32_t target_frequency_hz,
                                                  uint32_t timer_clock_hz,
                                                  uint32_t *prescaler,
                                                  uint32_t *period);

/**
 * @brief Calculate PWM duty cycle
 * @param duty_percent Duty cycle percentage (0-100)
 * @param period Timer period value
 * @return Pulse width value
 */
uint32_t blfm_timer_calculate_pwm_duty(uint8_t duty_percent, uint32_t period);

#endif // BLFM_TIMER_H