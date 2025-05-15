
#include "blfm_safety.h"
#include "FreeRTOS.h"
#include "blfm_alarm.h"
#include "blfm_led.h"
#include "blfm_motor.h"
#include "blfm_ultrasonic.h"
#include "task.h"

static uint16_t map_distance_to_blink_delay(uint16_t mm) {
  if (mm < 100)
    return 100; // Close → fast blink
  if (mm > 1000)
    return 1000; // Far → slow blink
  return mm;     // Linear mapping for demo
}

void blfm_safety_monitor_task(void *params) {

  (void)params;
  while (1) {
    uint16_t dist = blfm_ultrasonic_get_distance_mm();
    uint16_t delay_ms = map_distance_to_blink_delay(dist);

    blfm_led_set_blink_rate(delay_ms);

    if (dist < 150) {
      blfm_motor_stop();
      blfm_alarm_on();
    } else {
      blfm_motor_start();
      blfm_alarm_off();
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
