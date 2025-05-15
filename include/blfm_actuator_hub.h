#ifndef BLFM_ACTUATOR_HUB_H
#define BLFM_ACTUATOR_HUB_H

#include <stdint.h>

// Command types for actuators (example)
typedef enum {
    ACT_CMD_NONE = 0,
    ACT_CMD_MOTOR,
    ACT_CMD_ALARM,
    // add more actuator command types here
} ActuatorCommandType;

typedef struct {
    ActuatorCommandType type;
    union {
        struct {
            int16_t speed;   // motor speed -100 to 100
            int16_t direction; // -1 reverse, 1 forward
        } motor_cmd;

        struct {
            uint8_t on;     // alarm on/off
        } alarm_cmd;

        // Add other actuator command structures here
    } cmd;
} ActuatorCommand;

// Initialize actuator hub
void blfm_actuator_hub_init(void);

// Send command to actuators via hub
void blfm_actuator_hub_send_command(const ActuatorCommand *command);

void blfm_actuator_hub_start(void);

#endif // BLFM_ACTUATOR_HUB_H
