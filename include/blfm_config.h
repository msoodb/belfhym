#ifndef BLFM_CONFIG_H
#define BLFM_CONFIG_H

/* === Hardware presence flags === */
#define BLFM_LCD_ATTACHED             0   // Set to 1 if LCD is connected
#define BLFM_ULTRASONIC_ATTACHED      1   // Set to 0 if no ultrasonic sensor
#define BLFM_LED_ATTACHED             1   // Assume onboard LED is always present

/* === System-wide constants === */
#define BLFM_DEFAULT_TASK_PRIORITY    1
#define BLFM_DEFAULT_STACK_SIZE       256

/* === Feature toggles === */
#define ENABLE_DEBUG_PRINT            1
#define ENABLE_LOGGING                0

/* === Tunable parameters === */
#define BLFM_LED_BLINK_DEFAULT_MS     100
#define BLFM_CONTROL_LOOP_PERIOD_MS   50

#endif /* BLFM_CONFIG_H */
