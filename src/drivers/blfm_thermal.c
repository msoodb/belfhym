
#include "blfm_thermal.h"
#include "blfm_board.h"
#include "stm32f1xx.h"

#define THERMAL_ADC_CHANNEL BLFM_THERMAL_ADC_CHANNEL
#define ADC_MAX             4095
#define VREF_MV             3300



void blfm_thermal_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t blfm_thermal_get_value_raw(void) {
    ADC1->SQR3 = 1;
    ADC1->CR2 |= ADC_CR2_ADON;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

// Simulated function to read ADC value (replace with real ADC read logic)
static uint16_t blfm_read_adc(void) {
    // Select ADC channel
    ADC1->SQR3 = THERMAL_ADC_CHANNEL;

    // Start conversion
    ADC1->CR2 |= ADC_CR2_ADON;
    while (!(ADC1->SR & ADC_SR_EOC));

    return ADC1->DR;
}

// Return temperature in deci-Celsius (1 decimal precision), e.g. 255 => 25.5°C
int16_t blfm_thermal_get_temperature_dc(void) {
    uint16_t adc_value = blfm_read_adc();

    // Convert ADC reading to millivolts
    uint32_t voltage_mv = (adc_value * VREF_MV) / ADC_MAX;

    // LM35 outputs 10mV per degree Celsius => temp = V / 10
    // In deci-Celsius: temp_dc = (V_mv * 10) / 100
    int16_t temp_dc = (int16_t)((voltage_mv * 10) / 100);

    return temp_dc;
}
