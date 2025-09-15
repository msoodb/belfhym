/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Homa.
 *
 * Homa is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_TIMER_H
#define BLFM_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/*                          PUBLIC CONSTANTS                                 */
/* ========================================================================== */

#define TIMER_MAX_INSTANCES             4
#define TIMER_TIMEOUT_VALUE             100000
#define TIMER_INTERRUPT_PRIORITY        6

/* ========================================================================== */
/*                          PUBLIC ENUMERATIONS                              */
/* ========================================================================== */

/**
 * @brief Timer instances
 */
typedef enum {
    BLFM_TIMER1 = 0,
    BLFM_TIMER2 = 1,
    BLFM_TIMER3 = 2,
    BLFM_TIMER4 = 3
} blfm_timer_instance_t;

/**
 * @brief Timer status codes
 */
typedef enum {
    BLFM_TIMER_OK = 0,
    BLFM_TIMER_ERROR_INVALID_PARAM,
    BLFM_TIMER_ERROR_NOT_INITIALIZED,
    BLFM_TIMER_ERROR_ALREADY_INITIALIZED,
    BLFM_TIMER_ERROR_TIMEOUT
} blfm_timer_status_t;

/**
 * @brief Timer operating modes
 */
typedef enum {
    BLFM_TIMER_MODE_BASIC = 0,
    BLFM_TIMER_MODE_PWM = 1,
    BLFM_TIMER_MODE_INPUT_CAPTURE = 2,
    BLFM_TIMER_MODE_ENCODER = 3
} blfm_timer_mode_t;

/**
 * @brief Input capture edge selection
 */
typedef enum {
    BLFM_TIMER_CAPTURE_RISING = 0,
    BLFM_TIMER_CAPTURE_FALLING = 1,
    BLFM_TIMER_CAPTURE_BOTH = 2
} blfm_timer_capture_edge_t;

/**
 * @brief GPIO configuration modes for timer pins
 */
typedef enum {
    BLFM_TIMER_GPIO_AF_PP = 0,          /* Alternate function push-pull */
    BLFM_TIMER_GPIO_AF_OD = 1,          /* Alternate function open-drain */
    BLFM_TIMER_GPIO_INPUT_FLOATING = 2,  /* Input floating */
    BLFM_TIMER_GPIO_INPUT_PULLUP = 3    /* Input with pull-up */
} blfm_timer_gpio_mode_t;

/* ========================================================================== */
/*                          PUBLIC STRUCTURES                                */
/* ========================================================================== */

/**
 * @brief Timer configuration structure
 */
typedef struct {
    blfm_timer_mode_t mode;             /* Timer operating mode */
    uint32_t prescaler;                 /* Clock prescaler (1-65536) */
    uint32_t period;                    /* Timer period/ARR value */
    uint32_t frequency;                 /* Desired frequency (for PWM mode) */
    void (*callback)(void);             /* Interrupt callback function */
} blfm_timer_config_t;

/* ========================================================================== */
/*                          PUBLIC FUNCTION PROTOTYPES                       */
/* ========================================================================== */

/* ========================================================================== */
/*                          BASIC TIMER FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Initialize timer instance
 * @param instance Timer instance to initialize
 * @param config Configuration parameters
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_init(blfm_timer_instance_t instance, const blfm_timer_config_t *config);

/**
 * @brief Deinitialize timer instance
 * @param instance Timer instance to deinitialize
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_deinit(blfm_timer_instance_t instance);

/**
 * @brief Start timer
 * @param instance Timer instance
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_start(blfm_timer_instance_t instance);

/**
 * @brief Stop timer
 * @param instance Timer instance
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_stop(blfm_timer_instance_t instance);

/**
 * @brief Get timer counter value
 * @param instance Timer instance
 * @return Current counter value
 */
uint32_t blfm_timer_get_counter(blfm_timer_instance_t instance);

/**
 * @brief Set timer counter value
 * @param instance Timer instance
 * @param value Counter value to set
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_set_counter(blfm_timer_instance_t instance, uint32_t value);

/* ========================================================================== */
/*                          PWM FUNCTIONS                                    */
/* ========================================================================== */

/**
 * @brief Configure PWM channel
 * @param instance Timer instance
 * @param channel Channel number (1-4)
 * @param duty_cycle_percent Duty cycle in percent (0-100)
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_pwm_config(blfm_timer_instance_t instance, uint8_t channel, uint32_t duty_cycle_percent);

/**
 * @brief Set PWM duty cycle
 * @param instance Timer instance
 * @param channel Channel number (1-4)
 * @param duty_cycle_percent Duty cycle in percent (0-100)
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_pwm_set_duty_cycle(blfm_timer_instance_t instance, uint8_t channel, uint32_t duty_cycle_percent);

/* ========================================================================== */
/*                          INPUT CAPTURE FUNCTIONS                          */
/* ========================================================================== */

/**
 * @brief Configure input capture channel
 * @param instance Timer instance
 * @param channel Channel number (1-4)
 * @param edge Capture edge selection
 * @return BLFM_TIMER_OK on success, error code otherwise
 */
blfm_timer_status_t blfm_timer_input_capture_config(blfm_timer_instance_t instance, uint8_t channel, blfm_timer_capture_edge_t edge);

/**
 * @brief Get input capture value
 * @param instance Timer instance
 * @param channel Channel number (1-4)
 * @return Captured value
 */
uint32_t blfm_timer_get_capture(blfm_timer_instance_t instance, uint8_t channel);

/**
 * @brief Measure frequency using input capture
 * @param instance Timer instance
 * @param channel Channel number (1-4)
 * @return Measured frequency in Hz (0 if timeout)
 */
uint32_t blfm_timer_measure_frequency(blfm_timer_instance_t instance, uint8_t channel);

/* ========================================================================== */
/*                          CONVENIENCE MACROS                               */
/* ========================================================================== */

/**
 * @brief Basic timer configuration for periodic interrupts
 */
#define BLFM_TIMER_CONFIG_BASIC(freq_hz, cb) { \
    .mode = BLFM_TIMER_MODE_BASIC, \
    .prescaler = 72, \
    .period = (1000000 / (freq_hz)), \
    .frequency = (freq_hz), \
    .callback = (cb) \
}

/**
 * @brief PWM timer configuration
 */
#define BLFM_TIMER_CONFIG_PWM(freq_hz) { \
    .mode = BLFM_TIMER_MODE_PWM, \
    .prescaler = 0, \
    .period = 0, \
    .frequency = (freq_hz), \
    .callback = NULL \
}

/**
 * @brief Input capture timer configuration
 */
#define BLFM_TIMER_CONFIG_CAPTURE() { \
    .mode = BLFM_TIMER_MODE_INPUT_CAPTURE, \
    .prescaler = 72, \
    .period = 0xFFFF, \
    .frequency = 0, \
    .callback = NULL \
}

/**
 * @brief Encoder timer configuration
 */
#define BLFM_TIMER_CONFIG_ENCODER(max_count) { \
    .mode = BLFM_TIMER_MODE_ENCODER, \
    .prescaler = 1, \
    .period = (max_count), \
    .frequency = 0, \
    .callback = NULL \
}

/**
 * @brief Servo PWM configuration (50Hz, 1-2ms pulse width)
 */
#define BLFM_TIMER_CONFIG_SERVO() BLFM_TIMER_CONFIG_PWM(50)

/**
 * @brief Convert microseconds to timer ticks (assumes 1MHz timer clock)
 */
#define TIMER_US_TO_TICKS(us)           (us)

/**
 * @brief Convert timer ticks to microseconds (assumes 1MHz timer clock)
 */
#define TIMER_TICKS_TO_US(ticks)        (ticks)

/**
 * @brief Convert servo pulse width (1000-2000μs) to duty cycle percent
 */
#define SERVO_US_TO_DUTY_CYCLE(us)      (((us) - 1000) / 10)

#endif /* BLFM_TIMER_H */