/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */


#include "blfm_timer.h"
#include "stm32f1xx.h"
#include <stddef.h>


typedef struct {
    TIM_TypeDef *instance;
    bool initialized;
    blfm_timer_mode_t mode;
    uint32_t frequency;
    uint32_t period;
    
    void (*callback)(void);
    bool pwm_channels[4];
    
} timer_handle_t;


static timer_handle_t timer_handles[TIMER_MAX_INSTANCES] = {0};


static timer_handle_t* get_timer_handle(blfm_timer_instance_t instance);
static void timer_clock_enable(blfm_timer_instance_t instance);
static void timer_gpio_config(blfm_timer_instance_t instance, uint8_t channel, blfm_timer_gpio_mode_t gpio_mode);
static void timer_interrupt_config(blfm_timer_instance_t instance);
static uint32_t timer_get_clock_frequency(blfm_timer_instance_t instance);


blfm_timer_status_t blfm_timer_init(blfm_timer_instance_t instance, const blfm_timer_config_t *config) {
    if (instance >= TIMER_MAX_INSTANCES || !config) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;
    }
    
    timer_handle_t *handle = get_timer_handle(instance);
    if (handle->initialized) {
        return BLFM_TIMER_ERROR_ALREADY_INITIALIZED;
    }
    
    timer_clock_enable(instance);
    
    TIM_TypeDef *timer = handle->instance;
    timer->CR1 = 0;
    timer->CR2 = 0;
    timer->SMCR = 0;
    timer->DIER = 0;
    timer->SR = 0;
    timer->EGR = 0;
    timer->CCMR1 = 0;
    timer->CCMR2 = 0;
    timer->CCER = 0;
    timer->CNT = 0;
    timer->PSC = 0;
    timer->ARR = 0;
    uint32_t timer_clock = timer_get_clock_frequency(instance);
    uint32_t prescaler = 0;
    uint32_t period = 0;
    
    switch (config->mode) {
        case BLFM_TIMER_MODE_BASIC:
            prescaler = config->prescaler;
            period = config->period;
            break;
            
        case BLFM_TIMER_MODE_PWM:
            if (config->frequency > 0) {
                uint32_t total_counts = timer_clock / config->frequency;
                if (total_counts > 65535) {
                    prescaler = (total_counts / 65535) + 1;
                    period = total_counts / prescaler;
                } else {
                    prescaler = 1;
                    period = total_counts;
                }
            } else {
                prescaler = config->prescaler;
                period = config->period;
            }
            break;
            
        case BLFM_TIMER_MODE_INPUT_CAPTURE:
            prescaler = config->prescaler;
            period = 0xFFFF;
            break;
            
        case BLFM_TIMER_MODE_ENCODER:
            prescaler = 0;
            period = config->period > 0 ? config->period : 0xFFFF;
            break;
    }
    timer->PSC = prescaler - 1;
    timer->ARR = period - 1;
    switch (config->mode) {
        case BLFM_TIMER_MODE_BASIC:
            break;
            
        case BLFM_TIMER_MODE_PWM:
            break;
            
        case BLFM_TIMER_MODE_INPUT_CAPTURE:
            break;
            
        case BLFM_TIMER_MODE_ENCODER:
            timer->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
            break;
    }
    if (config->callback) {
        handle->callback = config->callback;
        timer_interrupt_config(instance);
        timer->DIER |= TIM_DIER_UIE;
    }
    timer->EGR |= TIM_EGR_UG;
    
    handle->mode = config->mode;
    handle->frequency = config->frequency;
    handle->period = period;
    handle->initialized = true;
    
    return BLFM_TIMER_OK;
}

blfm_timer_status_t blfm_timer_deinit(blfm_timer_instance_t instance) {
    if (instance >= TIMER_MAX_INSTANCES) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;
    }
    
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    handle->instance->CR1 = 0;
    
    handle->initialized = false;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Start timer
 */
blfm_timer_status_t blfm_timer_start(blfm_timer_instance_t instance) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    /* Clear pending interrupt flag */
    handle->instance->SR &= ~TIM_SR_UIF;
    
    /* Start timer */
    handle->instance->CR1 |= TIM_CR1_CEN;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Stop timer
 */
blfm_timer_status_t blfm_timer_stop(blfm_timer_instance_t instance) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    handle->instance->CR1 &= ~TIM_CR1_CEN;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Get timer counter value
 */
uint32_t blfm_timer_get_counter(blfm_timer_instance_t instance) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return 0;
    }
    
    return handle->instance->CNT;
}

/**
 * @brief Set timer counter value
 */
blfm_timer_status_t blfm_timer_set_counter(blfm_timer_instance_t instance, uint32_t value) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    handle->instance->CNT = value;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Configure PWM channel
 */
blfm_timer_status_t blfm_timer_pwm_config(blfm_timer_instance_t instance, uint8_t channel, uint32_t duty_cycle_percent) {
    if (channel == 0 || channel > 4 || duty_cycle_percent > 100) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;
    }
    
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    TIM_TypeDef *timer = handle->instance;
    
    /* Configure GPIO for PWM output */
    timer_gpio_config(instance, channel, BLFM_TIMER_GPIO_AF_PP);
    
    /* Calculate compare value */
    uint32_t compare_value = (handle->period * duty_cycle_percent) / 100;
    
    /* Configure channel */
    if (channel <= 2) {
        /* Channels 1-2: CCMR1 */
        uint32_t shift = (channel - 1) * 8;
        timer->CCMR1 &= ~(0xFF << shift);
        timer->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2) << shift;  /* PWM mode 1 */
        timer->CCMR1 |= TIM_CCMR1_OC1PE << shift;  /* Preload enable */
    } else {
        /* Channels 3-4: CCMR2 */
        uint32_t shift = (channel - 3) * 8;
        timer->CCMR2 &= ~(0xFF << shift);
        timer->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2) << shift;  /* PWM mode 1 */
        timer->CCMR2 |= TIM_CCMR2_OC3PE << shift;  /* Preload enable */
    }
    
    /* Set compare value */
    switch (channel) {
        case 1: timer->CCR1 = compare_value; break;
        case 2: timer->CCR2 = compare_value; break;
        case 3: timer->CCR3 = compare_value; break;
        case 4: timer->CCR4 = compare_value; break;
    }
    
    /* Enable channel */
    timer->CCER |= (TIM_CCER_CC1E << ((channel - 1) * 4));
    
    /* Enable auto-reload preload */
    timer->CR1 |= TIM_CR1_ARPE;
    
    handle->pwm_channels[channel - 1] = true;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Set PWM duty cycle
 */
blfm_timer_status_t blfm_timer_pwm_set_duty_cycle(blfm_timer_instance_t instance, uint8_t channel, uint32_t duty_cycle_percent) {
    if (channel == 0 || channel > 4 || duty_cycle_percent > 100) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;
    }
    
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    if (!handle->pwm_channels[channel - 1]) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;  /* Channel not configured for PWM */
    }
    
    /* Calculate compare value */
    uint32_t compare_value = (handle->period * duty_cycle_percent) / 100;
    
    /* Set compare value */
    switch (channel) {
        case 1: handle->instance->CCR1 = compare_value; break;
        case 2: handle->instance->CCR2 = compare_value; break;
        case 3: handle->instance->CCR3 = compare_value; break;
        case 4: handle->instance->CCR4 = compare_value; break;
    }
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Configure input capture channel
 */
blfm_timer_status_t blfm_timer_input_capture_config(blfm_timer_instance_t instance, uint8_t channel, blfm_timer_capture_edge_t edge) {
    if (channel == 0 || channel > 4) {
        return BLFM_TIMER_ERROR_INVALID_PARAM;
    }
    
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized) {
        return BLFM_TIMER_ERROR_NOT_INITIALIZED;
    }
    
    TIM_TypeDef *timer = handle->instance;
    
    /* Configure GPIO for input capture */
    timer_gpio_config(instance, channel, BLFM_TIMER_GPIO_INPUT_FLOATING);
    
    /* Configure channel for input capture */
    if (channel <= 2) {
        /* Channels 1-2: CCMR1 */
        uint32_t shift = (channel - 1) * 8;
        timer->CCMR1 &= ~(0xFF << shift);
        timer->CCMR1 |= (TIM_CCMR1_CC1S_0) << shift;  /* Input capture mode */
    } else {
        /* Channels 3-4: CCMR2 */
        uint32_t shift = (channel - 3) * 8;
        timer->CCMR2 &= ~(0xFF << shift);
        timer->CCMR2 |= (TIM_CCMR2_CC3S_0) << shift;  /* Input capture mode */
    }
    
    /* Configure capture edge */
    uint32_t ccer_shift = (channel - 1) * 4;
    timer->CCER &= ~(0xF << ccer_shift);
    
    switch (edge) {
        case BLFM_TIMER_CAPTURE_RISING:
            /* Rising edge - default, no additional bits */
            break;
        case BLFM_TIMER_CAPTURE_FALLING:
            timer->CCER |= (TIM_CCER_CC1P) << ccer_shift;
            break;
        case BLFM_TIMER_CAPTURE_BOTH:
            timer->CCER |= (TIM_CCER_CC1P | TIM_CCER_CC1NP) << ccer_shift;
            break;
    }
    
    /* Enable channel */
    timer->CCER |= (TIM_CCER_CC1E) << ccer_shift;
    
    return BLFM_TIMER_OK;
}

/**
 * @brief Get input capture value
 */
uint32_t blfm_timer_get_capture(blfm_timer_instance_t instance, uint8_t channel) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized || channel == 0 || channel > 4) {
        return 0;
    }
    
    switch (channel) {
        case 1: return handle->instance->CCR1;
        case 2: return handle->instance->CCR2;
        case 3: return handle->instance->CCR3;
        case 4: return handle->instance->CCR4;
        default: return 0;
    }
}

/**
 * @brief Measure frequency using input capture
 */
uint32_t blfm_timer_measure_frequency(blfm_timer_instance_t instance, uint8_t channel) {
    timer_handle_t *handle = get_timer_handle(instance);
    if (!handle || !handle->initialized || channel == 0 || channel > 4) {
        return 0;
    }
    
    TIM_TypeDef *timer = handle->instance;
    uint32_t capture_flag = TIM_SR_CC1IF << (channel - 1);
    
    /* Clear capture flag */
    timer->SR &= ~capture_flag;
    
    /* Wait for first capture */
    uint32_t timeout = TIMER_TIMEOUT_VALUE;
    while (!(timer->SR & capture_flag) && --timeout);
    if (timeout == 0) return 0;
    
    uint32_t capture1 = blfm_timer_get_capture(instance, channel);
    timer->SR &= ~capture_flag;
    
    /* Wait for second capture */
    timeout = TIMER_TIMEOUT_VALUE;
    while (!(timer->SR & capture_flag) && --timeout);
    if (timeout == 0) return 0;
    
    uint32_t capture2 = blfm_timer_get_capture(instance, channel);
    
    /* Calculate frequency */
    uint32_t timer_clock = timer_get_clock_frequency(instance);
    uint32_t prescaler = timer->PSC + 1;
    uint32_t period_ticks = (capture2 > capture1) ? (capture2 - capture1) : (0x10000 - capture1 + capture2);
    
    if (period_ticks > 0) {
        return (timer_clock / prescaler) / period_ticks;
    }
    
    return 0;
}

/* ========================================================================== */
/*                          INTERRUPT HANDLERS                               */
/* ========================================================================== */

/**
 * @brief TIM1 interrupt handler
 */
void TIM1_UP_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR &= ~TIM_SR_UIF;
        if (timer_handles[BLFM_TIMER1].callback) {
            timer_handles[BLFM_TIMER1].callback();
        }
    }
}

/**
 * @brief TIM2 interrupt handler
 */
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (timer_handles[BLFM_TIMER2].callback) {
            timer_handles[BLFM_TIMER2].callback();
        }
    }
}

/**
 * @brief TIM3 interrupt handler
 */
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        if (timer_handles[BLFM_TIMER3].callback) {
            timer_handles[BLFM_TIMER3].callback();
        }
    }
}

/**
 * @brief TIM4 interrupt handler
 */
void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;
        if (timer_handles[BLFM_TIMER4].callback) {
            timer_handles[BLFM_TIMER4].callback();
        }
    }
}

/* ========================================================================== */
/*                          PRIVATE FUNCTIONS                                */
/* ========================================================================== */

static timer_handle_t* get_timer_handle(blfm_timer_instance_t instance) {
    if (instance >= TIMER_MAX_INSTANCES) {
        return NULL;
    }
    
    timer_handle_t *handle = &timer_handles[instance];
    
    /* Initialize instance pointer if not set */
    if (!handle->instance) {
        switch (instance) {
            case BLFM_TIMER1:
                handle->instance = TIM1;
                break;
            case BLFM_TIMER2:
                handle->instance = TIM2;
                break;
            case BLFM_TIMER3:
                handle->instance = TIM3;
                break;
            case BLFM_TIMER4:
                handle->instance = TIM4;
                break;
        }
    }
    
    return handle;
}

static void timer_clock_enable(blfm_timer_instance_t instance) {
    switch (instance) {
        case BLFM_TIMER1:
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
            break;
        case BLFM_TIMER2:
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
            break;
        case BLFM_TIMER3:
            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
            break;
        case BLFM_TIMER4:
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
            break;
    }
}

static void timer_gpio_config(blfm_timer_instance_t instance, uint8_t channel, blfm_timer_gpio_mode_t gpio_mode) {
    GPIO_TypeDef *gpio_port = NULL;
    uint8_t pin = 0;
    uint32_t config = 0;
    
    /* Determine GPIO port and pin */
    switch (instance) {
        case BLFM_TIMER1:
            gpio_port = GPIOA;
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            pin = 7 + channel;  /* PA8-PA11 */
            break;
        case BLFM_TIMER2:
            gpio_port = GPIOA;
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            pin = channel - 1;  /* PA0-PA3 */
            break;
        case BLFM_TIMER3:
            gpio_port = GPIOA;
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            pin = 5 + channel;  /* PA6-PA7, PB0-PB1 */
            if (channel > 2) {
                gpio_port = GPIOB;
                RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
                pin = channel - 3;
            }
            break;
        case BLFM_TIMER4:
            gpio_port = GPIOB;
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            pin = 5 + channel;  /* PB6-PB9 */
            break;
    }
    
    if (!gpio_port || pin > 15) return;
    
    /* Configure GPIO mode */
    switch (gpio_mode) {
        case BLFM_TIMER_GPIO_AF_PP:
            config = 0xB;  /* Alternate function push-pull, 50MHz */
            break;
        case BLFM_TIMER_GPIO_AF_OD:
            config = 0xF;  /* Alternate function open-drain, 50MHz */
            break;
        case BLFM_TIMER_GPIO_INPUT_FLOATING:
            config = 0x4;  /* Input floating */
            break;
        case BLFM_TIMER_GPIO_INPUT_PULLUP:
            config = 0x8;  /* Input pull-up */
            gpio_port->ODR |= (1 << pin);
            break;
    }
    
    /* Configure pin */
    if (pin < 8) {
        /* CRL register */
        gpio_port->CRL &= ~(0xF << (pin * 4));
        gpio_port->CRL |= (config << (pin * 4));
    } else {
        /* CRH register */
        gpio_port->CRH &= ~(0xF << ((pin - 8) * 4));
        gpio_port->CRH |= (config << ((pin - 8) * 4));
    }
}

static void timer_interrupt_config(blfm_timer_instance_t instance) {
    IRQn_Type irq;
    
    switch (instance) {
        case BLFM_TIMER1:
            irq = TIM1_UP_IRQn;
            break;
        case BLFM_TIMER2:
            irq = TIM2_IRQn;
            break;
        case BLFM_TIMER3:
            irq = TIM3_IRQn;
            break;
        case BLFM_TIMER4:
            irq = TIM4_IRQn;
            break;
        default:
            return;
    }
    
    NVIC_SetPriority(irq, TIMER_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(irq);
}

static uint32_t timer_get_clock_frequency(blfm_timer_instance_t instance) {
    uint32_t pclk;
    
    switch (instance) {
        case BLFM_TIMER1:
            /* TIM1 is on APB2 */
            pclk = SystemCoreClock;  /* APB2 = HCLK */
            break;
        default:
            /* TIM2-4 are on APB1 */
            pclk = SystemCoreClock / 2;  /* APB1 = HCLK/2 */
            break;
    }
    
    return pclk;
}