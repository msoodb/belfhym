#ifndef BLFM_CONFIG_H
#define BLFM_CONFIG_H

/* === Hardware presence flags === */
#define BLFM_LCD_DISABLED 1
#define BLFM_ULTRASONIC_DISABLED 1
#define BLFM_LED_DISABLED 0
#define BLFM_TEMPRATURE_DISABLED 1
#define BLFM_MOTOR_DISABLED 1
#define BLFM_BIGSOUND_DISABLED 1
#define BLFM_ALARM_DISABLED 1
#define BLFM_RADIO_DISABLED 1

/* === System-wide constants === */
#define BLFM_DEFAULT_TASK_PRIORITY    1
#define BLFM_DEFAULT_STACK_SIZE       256

/* === Feature toggles === */
#define BLFM_DEBUG_ENABLED 0
#define ENABLE_DEBUG_PRINT            1
#define ENABLE_LOGGING                0

/* === Tunable parameters === */
#define BLFM_LED_BLINK_DEFAULT_MS     100
#define BLFM_CONTROL_LOOP_PERIOD_MS   50

#endif /* BLFM_CONFIG_H */
