#include "blfm_controller.h"
#include "blfm_motor.h"
#include "blfm_imu.h"
#include "blfm_ultrasonic.h"

void blfm_controller_run_cycle(void) {
  /*IMUData imu = read_imu_mock();
  UltrasonicData ultra = read_ultrasonic_mock();

  SafetyStatus safety = blfm_safety_check(&imu, &ultra);
  if (safety == SAFETY_OK) {
    DriveCommand cmd = blfm_pathfinder_compute(&imu, &ultra);
    blfm_motor_drive(&cmd);
  } else {
    blfm_motor_stop();
    }*/
}
