#include "blfm_safety.h"
#include "FreeRTOS.h"
#include "blfm_alarm.h"
#include "blfm_led.h"
#include "blfm_motor.h"
#include "blfm_ultrasonic.h"
#include "task.h"

#define DANGER_THRESHOLD_MM 200 // 20 cm

void blfm_safety_monitor_task(void *params) {

  (void)params;
  while (1) {
    uint16_t distance = blfm_ultrasonic_get_distance_mm();

    if (distance < DANGER_THRESHOLD_MM) {
      blfm_motor_stop();
      blfm_alarm_on();
      blfm_led_set_blink_rate(100);
    } else {
      blfm_motor_run();
      blfm_alarm_off();
      blfm_led_set_blink_rate(500);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
