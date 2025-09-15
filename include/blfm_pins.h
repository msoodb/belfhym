#ifndef BLFM_PINS_H
#define BLFM_PINS_H

/* SPI/NRF24 pins are defined in S17_config.h */

/* SERVO MODULES - TIM1 channels */
#define BLFM_SERVO1_PWM_PORT GPIOA
#define BLFM_SERVO1_PWM_PIN  8  /* TIM1_CH1 */

#define BLFM_SERVO2_PWM_PORT GPIOA
#define BLFM_SERVO2_PWM_PIN  9  /* TIM1_CH2 */

#define BLFM_SERVO3_PWM_PORT GPIOA
#define BLFM_SERVO3_PWM_PIN  10 /* TIM1_CH3 */

#define BLFM_SERVO4_PWM_PORT GPIOA
#define BLFM_SERVO4_PWM_PIN  11 /* TIM1_CH4 */

/* MOTOR MODULE - TIM4 PWM + Direction Control */
#define BLFM_MOTOR_ENA_PORT GPIOB
#define BLFM_MOTOR_ENA_PIN 6  /* TIM4_CH1 PWM - Left Motor Enable */

#define BLFM_MOTOR_ENB_PORT GPIOB
#define BLFM_MOTOR_ENB_PIN 7  /* TIM4_CH2 PWM - Right Motor Enable */

#define BLFM_MOTOR_IN1_PORT GPIOB
#define BLFM_MOTOR_IN1_PIN 8  /* Left Motor Direction 1 */

#define BLFM_MOTOR_IN2_PORT GPIOB
#define BLFM_MOTOR_IN2_PIN 9  /* Left Motor Direction 2 */

#define BLFM_MOTOR_IN3_PORT GPIOB
#define BLFM_MOTOR_IN3_PIN 0  /* Right Motor Direction 1 */

#define BLFM_MOTOR_IN4_PORT GPIOB
#define BLFM_MOTOR_IN4_PIN 1  /* Right Motor Direction 2 */

/* ULTRASONIC MODULE */
#define BLFM_ULTRASONIC_ECHO_PORT GPIOB
#define BLFM_ULTRASONIC_ECHO_PIN 11

#define BLFM_ULTRASONIC_TRIG_PORT GPIOB
#define BLFM_ULTRASONIC_TRIG_PIN 12


/* LED MODULE - Only onboard LED */
#define BLFM_LED_ONBOARD_PORT GPIOC
#define BLFM_LED_ONBOARD_PIN 13

#endif