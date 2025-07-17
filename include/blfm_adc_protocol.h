/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef BLFM_ADC_PROTOCOL_H
#define BLFM_ADC_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief ADC peripheral enumeration
 */
typedef enum {
  BLFM_ADC1 = 0,
  BLFM_ADC2,
  BLFM_ADC_COUNT
} blfm_adc_peripheral_t;

/**
 * @brief ADC channel enumeration
 */
typedef enum {
  BLFM_ADC_CHANNEL_0 = 0,
  BLFM_ADC_CHANNEL_1,
  BLFM_ADC_CHANNEL_2,
  BLFM_ADC_CHANNEL_3,
  BLFM_ADC_CHANNEL_4,
  BLFM_ADC_CHANNEL_5,
  BLFM_ADC_CHANNEL_6,
  BLFM_ADC_CHANNEL_7,
  BLFM_ADC_CHANNEL_8,
  BLFM_ADC_CHANNEL_9,
  BLFM_ADC_CHANNEL_10,
  BLFM_ADC_CHANNEL_11,
  BLFM_ADC_CHANNEL_12,
  BLFM_ADC_CHANNEL_13,
  BLFM_ADC_CHANNEL_14,
  BLFM_ADC_CHANNEL_15,
  BLFM_ADC_CHANNEL_TEMP,      // Temperature sensor
  BLFM_ADC_CHANNEL_VREF       // Internal reference voltage
} blfm_adc_channel_t;

/**
 * @brief ADC sampling time enumeration
 */
typedef enum {
  BLFM_ADC_SAMPLE_1_5_CYCLES = 0,
  BLFM_ADC_SAMPLE_7_5_CYCLES,
  BLFM_ADC_SAMPLE_13_5_CYCLES,
  BLFM_ADC_SAMPLE_28_5_CYCLES,
  BLFM_ADC_SAMPLE_41_5_CYCLES,
  BLFM_ADC_SAMPLE_55_5_CYCLES,
  BLFM_ADC_SAMPLE_71_5_CYCLES,
  BLFM_ADC_SAMPLE_239_5_CYCLES
} blfm_adc_sample_time_t;

/**
 * @brief ADC conversion mode enumeration
 */
typedef enum {
  BLFM_ADC_MODE_SINGLE = 0,
  BLFM_ADC_MODE_CONTINUOUS,
  BLFM_ADC_MODE_SCAN,
  BLFM_ADC_MODE_DISCONTINUOUS
} blfm_adc_conversion_mode_t;

/**
 * @brief ADC trigger source enumeration
 */
typedef enum {
  BLFM_ADC_TRIGGER_SOFTWARE = 0,
  BLFM_ADC_TRIGGER_TIM1_CC1,
  BLFM_ADC_TRIGGER_TIM1_CC2,
  BLFM_ADC_TRIGGER_TIM1_CC3,
  BLFM_ADC_TRIGGER_TIM2_CC2,
  BLFM_ADC_TRIGGER_TIM3_TRGO,
  BLFM_ADC_TRIGGER_TIM4_CC4,
  BLFM_ADC_TRIGGER_EXTI_LINE11
} blfm_adc_trigger_t;

/**
 * @brief ADC channel configuration
 */
typedef struct {
  blfm_adc_channel_t channel;
  blfm_adc_sample_time_t sample_time;
  uint8_t rank;                   // Conversion rank (1-16)
} blfm_adc_channel_config_t;

/**
 * @brief ADC configuration structure
 */
typedef struct {
  blfm_adc_conversion_mode_t mode;
  blfm_adc_trigger_t trigger;
  bool data_align_right;          // true = right align, false = left align
  uint8_t num_channels;           // Number of channels in sequence
  blfm_adc_channel_config_t *channels;  // Channel configurations
  uint32_t timeout_ms;
} blfm_adc_config_t;

/**
 * @brief ADC error codes
 */
typedef enum {
  BLFM_ADC_OK = 0,
  BLFM_ADC_ERR_INVALID_PERIPHERAL,
  BLFM_ADC_ERR_INVALID_CHANNEL,
  BLFM_ADC_ERR_INVALID_CONFIG,
  BLFM_ADC_ERR_NOT_INITIALIZED,
  BLFM_ADC_ERR_TIMEOUT,
  BLFM_ADC_ERR_NULL_PTR,
  BLFM_ADC_ERR_BUSY,
  BLFM_ADC_ERR_CALIBRATION_FAILED
} blfm_adc_error_t;

/**
 * @brief ADC callback function type
 */
typedef void (*blfm_adc_callback_t)(blfm_adc_peripheral_t adc, uint16_t *data, uint8_t length);

/**
 * @brief Initialize ADC peripheral
 * @param peripheral ADC peripheral to initialize
 * @param config Configuration structure (can be NULL for default)
 * @return Error code
 */
blfm_adc_error_t blfm_adc_protocol_init(blfm_adc_peripheral_t peripheral, const blfm_adc_config_t *config);

/**
 * @brief Deinitialize ADC peripheral
 * @param peripheral ADC peripheral to deinitialize
 * @return Error code
 */
blfm_adc_error_t blfm_adc_protocol_deinit(blfm_adc_peripheral_t peripheral);

/**
 * @brief Calibrate ADC peripheral
 * @param peripheral ADC peripheral to calibrate
 * @return Error code
 */
blfm_adc_error_t blfm_adc_calibrate(blfm_adc_peripheral_t peripheral);

/**
 * @brief Start single conversion
 * @param peripheral ADC peripheral
 * @param channel Channel to convert
 * @param result Pointer to store result
 * @return Error code
 */
blfm_adc_error_t blfm_adc_convert_single(blfm_adc_peripheral_t peripheral, 
                                         blfm_adc_channel_t channel, 
                                         uint16_t *result);

/**
 * @brief Start continuous conversion
 * @param peripheral ADC peripheral
 * @param callback Callback function for results
 * @return Error code
 */
blfm_adc_error_t blfm_adc_start_continuous(blfm_adc_peripheral_t peripheral, 
                                           blfm_adc_callback_t callback);

/**
 * @brief Stop continuous conversion
 * @param peripheral ADC peripheral
 * @return Error code
 */
blfm_adc_error_t blfm_adc_stop_continuous(blfm_adc_peripheral_t peripheral);

/**
 * @brief Start scan conversion
 * @param peripheral ADC peripheral
 * @param results Buffer to store results
 * @param num_channels Number of channels to scan
 * @return Error code
 */
blfm_adc_error_t blfm_adc_start_scan(blfm_adc_peripheral_t peripheral, 
                                     uint16_t *results, 
                                     uint8_t num_channels);

/**
 * @brief Check if conversion is complete
 * @param peripheral ADC peripheral
 * @return true if conversion is complete
 */
bool blfm_adc_is_conversion_complete(blfm_adc_peripheral_t peripheral);

/**
 * @brief Get conversion result
 * @param peripheral ADC peripheral
 * @return Conversion result
 */
uint16_t blfm_adc_get_result(blfm_adc_peripheral_t peripheral);

/**
 * @brief Configure channel sampling time
 * @param peripheral ADC peripheral
 * @param channel Channel to configure
 * @param sample_time Sampling time
 * @return Error code
 */
blfm_adc_error_t blfm_adc_configure_channel(blfm_adc_peripheral_t peripheral, 
                                            blfm_adc_channel_t channel, 
                                            blfm_adc_sample_time_t sample_time);

/**
 * @brief Read internal temperature sensor
 * @param peripheral ADC peripheral
 * @param temperature_c Pointer to store temperature in Celsius
 * @return Error code
 */
blfm_adc_error_t blfm_adc_read_temperature(blfm_adc_peripheral_t peripheral, 
                                           float *temperature_c);

/**
 * @brief Read internal reference voltage
 * @param peripheral ADC peripheral
 * @param vref_mv Pointer to store reference voltage in millivolts
 * @return Error code
 */
blfm_adc_error_t blfm_adc_read_vref(blfm_adc_peripheral_t peripheral, 
                                    uint16_t *vref_mv);

/**
 * @brief Convert ADC value to voltage
 * @param adc_value ADC conversion result
 * @param vref_mv Reference voltage in millivolts
 * @param voltage_mv Pointer to store voltage in millivolts
 * @return Error code
 */
blfm_adc_error_t blfm_adc_to_voltage(uint16_t adc_value, 
                                     uint16_t vref_mv, 
                                     uint16_t *voltage_mv);

/**
 * @brief ADC interrupt handler (should be called from ADC1_2_IRQHandler)
 * @param peripheral ADC peripheral
 */
void blfm_adc_interrupt_handler(blfm_adc_peripheral_t peripheral);

#endif // BLFM_ADC_PROTOCOL_H