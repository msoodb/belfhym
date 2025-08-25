/**
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 * This file is part of Belfhym.
 */

#include "blfm_servomotor.h"
#include "blfm_gpio.h"
#include "blfm_pins.h"
#include "stm32f1xx.h"
#include <stdbool.h>

/* Servo Hardware Configuration - Uses TIM1 for dedicated servo control */
#define SERVO_TIMER TIM1
#define SERVO_FREQUENCY_HZ 50           /* Standard servo frequency */
#define SERVO_PERIOD_US 20000           /* 20ms period for 50Hz */
#define SERVO_TIMER_CLOCK_HZ 1000000    /* 1MHz for microsecond precision */

/* Servo pulse specifications - match working code */
#define SERVO_MIN_PULSE_US 500          /* Working code minimum */
#define SERVO_MAX_PULSE_US 2500         /* Working code maximum */
#define SERVO_CENTER_PULSE_US 1500      /* Working code center */
#define SERVO_PULSE_RANGE_US 1000       /* ±1000us from center */

/* Safety and performance parameters */
#define SERVO_MAX_CHANNELS 4
#define SERVO_MIN_ANGLE_X10 -900        /* Minimum angle * 10 (-90.0 degrees) */
#define SERVO_MAX_ANGLE_X10 900         /* Maximum angle * 10 (90.0 degrees) */
#define SERVO_DEADBAND_X10 20           /* Deadband * 10 (2.0 degrees) */
#define SERVO_MAX_SPEED_X10_MS 5        /* Maximum speed * 10 (0.5 deg/ms) */
#define SERVO_UPDATE_RATE_MS 20         /* Update rate for smooth movement */

/* Servo channel to pin mapping (TIM1 channels) */
typedef struct {
    GPIO_TypeDef *port;
    uint8_t pin;
    uint8_t af_config;  /* Alternate function config bits */
} servo_pin_t;

static const servo_pin_t servo_pins[SERVO_MAX_CHANNELS] = {
    {GPIOA, 8,  0xB},  /* TIM1_CH1 -> PA8  */
    {GPIOA, 9,  0xB},  /* TIM1_CH2 -> PA9  */
    {GPIOA, 10, 0xB},  /* TIM1_CH3 -> PA10 */
    {GPIOA, 11, 0xB}   /* TIM1_CH4 -> PA11 */
};

/* Servo state tracking (using fixed-point math, angle * 10) */
typedef struct {
    blfm_servo_type_t type;
    int16_t current_angle_x10;  /* Current position * 10 (decidegrees) */
    int16_t target_angle_x10;   /* Target position * 10 (decidegrees) */
    int16_t trim_us;           /* Trim adjustment in microseconds */
    uint16_t current_pulse_us; /* Current pulse width */
    bool enabled;
    bool reverse_direction;
    bool smooth_enabled;
    uint32_t last_update_ms;
} servo_state_t;

static servo_state_t servo_states[SERVO_MAX_CHANNELS];
static bool servo_system_initialized = false;
static uint32_t system_time_ms = 0;

/* Internal function declarations */
static void configure_servo_timer(void);
static void configure_servo_gpio(void);
static uint16_t angle_x10_to_pulse_us(int16_t angle_x10, int16_t trim_us);
static int16_t pulse_us_to_angle_x10(uint16_t pulse_us);
static void set_servo_pulse_direct(uint8_t channel, uint16_t pulse_us);
static int16_t apply_smooth_movement(servo_state_t *servo, int16_t target_angle_x10, uint32_t delta_ms);
static bool is_pulse_valid(uint16_t pulse_us);

/* Initialize servo control system */
void blfm_servomotor_init(void) {
    /* Reset all servo states */
    for (uint8_t i = 0; i < SERVO_MAX_CHANNELS; i++) {
        servo_states[i] = (servo_state_t) {
            .type = BLFM_SERVO_TYPE_MANUAL,
            .current_angle_x10 = 0,
            .target_angle_x10 = 0,
            .trim_us = 0,
            .current_pulse_us = SERVO_CENTER_PULSE_US,
            .enabled = false,
            .reverse_direction = false,
            .smooth_enabled = true,
            .last_update_ms = 0
        };
    }
    
    /* Configure hardware */
    configure_servo_gpio();
    configure_servo_timer();
    
    /* Set all servos to center position initially */
    for (uint8_t i = 0; i < SERVO_MAX_CHANNELS; i++) {
        set_servo_pulse_direct(i, SERVO_CENTER_PULSE_US);
    }
    
    servo_system_initialized = true;
    system_time_ms = 0;
}

/* Configure GPIO pins for servo PWM output */
static void configure_servo_gpio(void) {
    /* Enable required clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    
    /* Configure servo pins as alternate function push-pull */
    for (uint8_t i = 0; i < SERVO_MAX_CHANNELS; i++) {
        const servo_pin_t *pin = &servo_pins[i];
        
        /* Configure pin as AF push-pull, 50MHz */
        if (pin->pin < 8) {
            /* CRL register for pins 0-7 */
            pin->port->CRL &= ~(0xF << (pin->pin * 4));
            pin->port->CRL |= (pin->af_config << (pin->pin * 4));
        } else {
            /* CRH register for pins 8-15 */
            uint8_t pin_offset = pin->pin - 8;
            pin->port->CRH &= ~(0xF << (pin_offset * 4));
            pin->port->CRH |= (pin->af_config << (pin_offset * 4));
        }
    }
}

/* Configure TIM1 for servo PWM generation */
static void configure_servo_timer(void) {
    /* Enable TIM1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    
    /* Calculate prescaler for 1MHz timer clock (72MHz / 72 = 1MHz) */
    uint32_t prescaler = 71;  /* Fixed prescaler for 1MHz from 72MHz system clock */
    
    /* Stop timer during configuration */
    SERVO_TIMER->CR1 = 0;
    
    /* Configure timer */
    SERVO_TIMER->PSC = prescaler;                    /* 1MHz timer clock */
    SERVO_TIMER->ARR = SERVO_PERIOD_US - 1;         /* 20ms period for 50Hz */
    SERVO_TIMER->CCR1 = SERVO_CENTER_PULSE_US;      /* Channel 1 center */
    SERVO_TIMER->CCR2 = SERVO_CENTER_PULSE_US;      /* Channel 2 center */
    SERVO_TIMER->CCR3 = SERVO_CENTER_PULSE_US;      /* Channel 3 center */
    SERVO_TIMER->CCR4 = SERVO_CENTER_PULSE_US;      /* Channel 4 center */
    
    /* Configure PWM mode 1 for all channels */
    SERVO_TIMER->CCMR1 = (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE |  /* CH1 PWM1 + preload */
                         (6 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;   /* CH2 PWM1 + preload */
    
    SERVO_TIMER->CCMR2 = (6 << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE |  /* CH3 PWM1 + preload */
                         (6 << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;   /* CH4 PWM1 + preload */
    
    /* Configure output compare and enable channels (disabled initially) */
    SERVO_TIMER->CCER = 0;  /* All channels disabled initially */
    
    /* Advanced timer specific - main output enable */
    SERVO_TIMER->BDTR = TIM_BDTR_MOE;
    
    /* Enable auto-reload preload and start timer */
    SERVO_TIMER->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
    
    /* Force update to load all settings */
    SERVO_TIMER->EGR = TIM_EGR_UG;
}

/* Enable servo channel */
void blfm_servomotor_enable(uint8_t channel) {
    if (channel >= SERVO_MAX_CHANNELS || !servo_system_initialized) return;
    
    servo_states[channel].enabled = true;
    
    
    /* Enable timer channel output */
    SERVO_TIMER->CCER |= (TIM_CCER_CC1E << (channel * 4));
}

/* Disable servo channel */
void blfm_servomotor_disable(uint8_t channel) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    
    servo_states[channel].enabled = false;
    
    /* Disable timer channel output */
    SERVO_TIMER->CCER &= ~(TIM_CCER_CC1E << (channel * 4));
}

/* Set servo type */
void blfm_servomotor_set_type(uint8_t channel, blfm_servo_type_t type) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    servo_states[channel].type = type;
}

/* Set servo direction reversal */
void blfm_servomotor_set_reverse(uint8_t channel, bool reverse) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    servo_states[channel].reverse_direction = reverse;
}

/* Set servo trim adjustment */
void blfm_servomotor_set_trim(uint8_t channel, int16_t trim_us) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    
    /* Limit trim to reasonable range */
    if (trim_us < -200) trim_us = -200;
    if (trim_us > 200) trim_us = 200;
    
    servo_states[channel].trim_us = trim_us;
}

/* Enable/disable smooth movement */
void blfm_servomotor_set_smooth(uint8_t channel, bool smooth) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    servo_states[channel].smooth_enabled = smooth;
}

/* Update system time (should be called from main loop) */
void blfm_servomotor_update_time(uint32_t current_time_ms) {
    system_time_ms = current_time_ms;
}

/* Apply servo command */
void blfm_servomotor_apply(uint8_t channel, const blfm_servomotor_command_t *cmd) {
    if (!cmd || channel >= SERVO_MAX_CHANNELS || !servo_states[channel].enabled) {
        return;
    }
    
    /* Check for special "no change" value (0x7FFF) */
    if (cmd->proportional_input == 0x7FFF) {
        return;  /* Don't change this servo */
    }
    
    
    servo_state_t *servo = &servo_states[channel];
    int16_t target_angle_x10 = 0;
    
    /* Determine target angle based on servo type */
    switch (servo->type) {
        case BLFM_SERVO_TYPE_MANUAL:
            target_angle_x10 = (int16_t)cmd->angle * 10;
            break;
            
        case BLFM_SERVO_TYPE_PROPORTIONAL: {
            /* Map proportional input (-1000 to +1000) to angle range (-900 to +900) */
            int16_t input = cmd->proportional_input;
            if (input < -1000) input = -1000;
            if (input > 1000) input = 1000;
            /* target_angle_x10 = (input * 900) / 1000 but use wider pulse range */
            target_angle_x10 = (int16_t)((int32_t)input * 900 / 1000);
            break;
        }
        
        case BLFM_SERVO_TYPE_STATIC:
            /* Static servo - don't change position */
            return;
            
        default:
            return;
    }
    
    /* Apply direction reversal */
    if (servo->reverse_direction) {
        target_angle_x10 = -target_angle_x10;
    }
    
    /* Clamp to valid angle range */
    if (target_angle_x10 < SERVO_MIN_ANGLE_X10) target_angle_x10 = SERVO_MIN_ANGLE_X10;
    if (target_angle_x10 > SERVO_MAX_ANGLE_X10) target_angle_x10 = SERVO_MAX_ANGLE_X10;
    
    /* Update target and apply movement */
    servo->target_angle_x10 = target_angle_x10;
    blfm_servomotor_update(channel);
}

/* Update servo position (smooth movement) */
void blfm_servomotor_update(uint8_t channel) {
    if (channel >= SERVO_MAX_CHANNELS || !servo_states[channel].enabled) {
        return;
    }
    
    servo_state_t *servo = &servo_states[channel];
    uint32_t current_time = system_time_ms;
    uint32_t delta_ms = current_time - servo->last_update_ms;
    
    /* Apply smooth movement if enabled */
    int16_t actual_angle_x10 = servo->target_angle_x10;
    if (servo->smooth_enabled && delta_ms > 0) {
        actual_angle_x10 = apply_smooth_movement(servo, servo->target_angle_x10, delta_ms);
    }
    
    servo->current_angle_x10 = actual_angle_x10;
    servo->last_update_ms = current_time;
    
    /* Convert to pulse width and apply */
    uint16_t pulse_us = angle_x10_to_pulse_us(actual_angle_x10, servo->trim_us);
    servo->current_pulse_us = pulse_us;
    
    
    set_servo_pulse_direct(channel, pulse_us);
}

/* Set servo to specific angle */
void blfm_servomotor_set_angle(uint8_t channel, int16_t angle_x10) {
    if (channel >= SERVO_MAX_CHANNELS || !servo_states[channel].enabled) {
        return;
    }
    
    /* Clamp angle to valid range */
    if (angle_x10 < SERVO_MIN_ANGLE_X10) angle_x10 = SERVO_MIN_ANGLE_X10;
    if (angle_x10 > SERVO_MAX_ANGLE_X10) angle_x10 = SERVO_MAX_ANGLE_X10;
    
    /* Apply direction reversal */
    if (servo_states[channel].reverse_direction) {
        angle_x10 = -angle_x10;
    }
    
    servo_states[channel].target_angle_x10 = angle_x10;
    blfm_servomotor_update(channel);
}

/* Set servo pulse width directly */
void blfm_servomotor_set_pulse_us(uint8_t channel, uint16_t pulse_us) {
    if (channel >= SERVO_MAX_CHANNELS || !servo_states[channel].enabled) {
        return;
    }
    
    if (!is_pulse_valid(pulse_us)) {
        return;  /* Safety check - reject invalid pulse widths */
    }
    
    servo_states[channel].current_pulse_us = pulse_us;
    servo_states[channel].current_angle_x10 = pulse_us_to_angle_x10(pulse_us);
    servo_states[channel].target_angle_x10 = servo_states[channel].current_angle_x10;
    
    set_servo_pulse_direct(channel, pulse_us);
}

/* Get current servo status */
void blfm_servomotor_get_status(uint8_t channel, blfm_servo_status_t *status) {
    if (!status || channel >= SERVO_MAX_CHANNELS) return;
    
    const servo_state_t *servo = &servo_states[channel];
    status->current_angle_x10 = servo->current_angle_x10;
    status->target_angle_x10 = servo->target_angle_x10;
    status->current_pulse_us = servo->current_pulse_us;
    status->enabled = servo->enabled;
    status->type = servo->type;
}

/* Emergency stop all servos */
void blfm_servomotor_emergency_stop(void) {
    /* Disable all channel outputs immediately */
    SERVO_TIMER->CCER = 0;
    
    /* Mark all servos as disabled */
    for (uint8_t i = 0; i < SERVO_MAX_CHANNELS; i++) {
        servo_states[i].enabled = false;
    }
}

/* Convert angle*10 to pulse width with trim */
static uint16_t angle_x10_to_pulse_us(int16_t angle_x10, int16_t trim_us) {
    /* Clamp angle to valid range */
    if (angle_x10 < SERVO_MIN_ANGLE_X10) angle_x10 = SERVO_MIN_ANGLE_X10;
    if (angle_x10 > SERVO_MAX_ANGLE_X10) angle_x10 = SERVO_MAX_ANGLE_X10;
    
    /* Convert angle*10 to pulse width - MATCH WORKING CODE FORMULA */
    /* Working formula: 1500 + (angle * 1000) / 90 */
    /* For angle_x10: 1500 + (angle_x10 * 1000) / 900 */
    /* This maps -900 (-90°) to 500us, 0 (0°) to 1500us, +900 (+90°) to 2500us */
    int32_t pulse = SERVO_CENTER_PULSE_US + ((int32_t)angle_x10 * SERVO_PULSE_RANGE_US) / 900;
    
    /* Apply trim adjustment */
    pulse += trim_us;
    
    /* Clamp to valid servo range */
    if (pulse < SERVO_MIN_PULSE_US) pulse = SERVO_MIN_PULSE_US;
    if (pulse > SERVO_MAX_PULSE_US) pulse = SERVO_MAX_PULSE_US;
    
    return (uint16_t)pulse;
}

/* Convert pulse width to angle*10 */
static int16_t pulse_us_to_angle_x10(uint16_t pulse_us) {
    /* Clamp pulse to valid range */
    if (pulse_us < SERVO_MIN_PULSE_US) pulse_us = SERVO_MIN_PULSE_US;
    if (pulse_us > SERVO_MAX_PULSE_US) pulse_us = SERVO_MAX_PULSE_US;
    
    /* Convert pulse width to angle*10 */
    /* angle_x10 = (pulse_offset * 900) / 928 */
    int32_t pulse_offset = (int32_t)pulse_us - SERVO_CENTER_PULSE_US;
    return (int16_t)((pulse_offset * 900) / SERVO_PULSE_RANGE_US);
}

/* Apply smooth movement with speed limiting */
static int16_t apply_smooth_movement(servo_state_t *servo, int16_t target_angle_x10, uint32_t delta_ms) {
    int16_t current = servo->current_angle_x10;
    int16_t difference = target_angle_x10 - current;
    
    /* Check if already close enough (deadband) */
    if (difference < SERVO_DEADBAND_X10 && difference > -SERVO_DEADBAND_X10) {
        return target_angle_x10;
    }
    
    /* Calculate maximum movement for this time step */
    int16_t max_movement = (int16_t)(SERVO_MAX_SPEED_X10_MS * delta_ms);
    
    /* Limit movement speed */
    if (difference > max_movement) {
        return current + max_movement;
    } else if (difference < -max_movement) {
        return current - max_movement;
    } else {
        return target_angle_x10;
    }
}

/* Set servo pulse width directly to hardware */
static void set_servo_pulse_direct(uint8_t channel, uint16_t pulse_us) {
    if (channel >= SERVO_MAX_CHANNELS) return;
    
    /* Clamp pulse to timer period */
    if (pulse_us > SERVO_PERIOD_US - 1) {
        pulse_us = SERVO_PERIOD_US - 1;
    }
    
    /* Set compare register for the channel */
    switch (channel) {
        case 0: SERVO_TIMER->CCR1 = pulse_us; break;
        case 1: SERVO_TIMER->CCR2 = pulse_us; break;
        case 2: SERVO_TIMER->CCR3 = pulse_us; break;
        case 3: SERVO_TIMER->CCR4 = pulse_us; break;
    }
}

/* Validate pulse width for safety */
static bool is_pulse_valid(uint16_t pulse_us) {
    return (pulse_us >= SERVO_MIN_PULSE_US && pulse_us <= SERVO_MAX_PULSE_US);
}