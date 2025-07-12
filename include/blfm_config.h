#ifndef BLFM_CONFIG_H
#define BLFM_CONFIG_H

/* === Sensors presence flags === */
#define BLFM_ENABLED_ULTRASONIC 0
#define BLFM_ENABLED_POTENTIOMETER 0
#define BLFM_ENABLED_TEMPERATURE 0
#define BLFM_ENABLED_JOYSTICK 0

/* === Actuators presence flags === */
#define BLFM_ENABLED_LED 1
#define BLFM_ENABLED_MOTOR 0
#define BLFM_ENABLED_DISPLAY 0
#define BLFM_ENABLED_ALARM 0
#define BLFM_ENABLED_SERVO 0
#define BLFM_ENABLED_RADIO 0

/* === Optional input/event features === */
#define BLFM_ENABLED_BIGSOUND 0
#define BLFM_ENABLED_IR_REMOTE 0
#define BLFM_ENABLED_MODE_BUTTON 1
#define BLFM_ENABLED_ESP32 0

/* === System-wide constants === */
// #define BLFM_DEFAULT_TASK_PRIORITY    1
// #define BLFM_DEFAULT_STACK_SIZE       256

/* === Feature toggles === */
// #define BLFM_DEBUG_ENABLED 1
// #define ENABLE_DEBUG_PRINT            1
// #define ENABLE_LOGGING                0

/* === Tunable parameters === */
// #define BLFM_LED_BLINK_DEFAULT_MS     100
// #define BLFM_CONTROL_LOOP_PERIOD_MS   50

#endif /* BLFM_CONFIG_H */
