#include "blfm_actuator_hub.h"
#include "blfm_alarm.h"
#include "blfm_motor.h"

void blfm_actuator_hub_init(void) {
  blfm_motor_init();
  blfm_alarm_init();
  // Init other actuators here
}

void blfm_actuator_hub_send_command(const ActuatorCommand *command) {
  switch (command->type) {
  case ACT_CMD_MOTOR:
    blfm_motor_set_speed_direction(command->cmd.motor_cmd.speed,
                                   command->cmd.motor_cmd.direction);
    break;
  case ACT_CMD_ALARM:
    if (command->cmd.alarm_cmd.on) {
      blfm_alarm_on();
    } else {
      blfm_alarm_off();
    }
    break;
  // Handle other actuators here
  default:
    // Unknown or none command, ignore or log
    break;
  }
}

void blfm_actuator_hub_start(void) {
  //xTaskCreate(blfm_motor_task, "Motor", 128, NULL, 2, NULL);
  //xTaskCreate(blfm_alarm_task, "Alarm", 128, NULL, 2, NULL);
}
