
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_controller.h"
#include "blfm_types.h"
#include "blfm_gpio.h"
#include <stdbool.h>

#define LED_EXTERNAL_PORT GPIOB
#define LED_EXTERNAL_PIN 5


extern char *strcpy(char *dest, const char *src);

void blfm_controller_init(void) {
  // Reserved for future initialization
  blfm_gpio_config_output((uint32_t)GPIOB, 11);
}

// Fill 'out' based on 'in'
void blfm_controller_process(const blfm_sensor_data_t *in,
                             blfm_actuator_command_t *out) {

  if (!in || !out)
    return;

  // Default motor values
  out->motor.speed = 0;
  out->motor.direction = 0;

  if (in->ultrasonic.distance_mm >= 200) {
    out->motor.speed = 128;
    out->motor.direction = 1;
  }

  out->led.mode = BLFM_LED_MODE_BLINK;
  out->led.blink_speed_ms = in->ultrasonic.distance_mm;

  if (in->ultrasonic.distance_mm < 100) {
    out->alarm.active = true;
    out->alarm.pattern_id = 1;
    out->alarm.duration_ms = 500;
    out->alarm.volume = 10;
  } else {
    out->alarm.active = false;
  }

  // Build display line1 = "Dist: xxx mm"
  char buf1[17] = {0};
  strcpy(buf1, "Dist: ");
  int dist = in->ultrasonic.distance_mm;
  int pos = 6;

  if (dist >= 100) {
    buf1[pos++] = '0' + (dist / 100) % 10;
    buf1[pos++] = '0' + (dist / 10) % 10;
    buf1[pos++] = '0' + dist % 10;
  } else if (dist >= 10) {
    buf1[pos++] = '0' + (dist / 10) % 10;
    buf1[pos++] = '0' + dist % 10;
  } else {
    buf1[pos++] = '0' + dist;
  }

  strcpy(&buf1[pos], " mm");

  // Build display line2 = "Speed: xxx"
  char buf2[17] = {0};
  strcpy(buf2, "Speed: ");
  int speed = out->motor.speed;
  pos = 7;

  if (speed >= 100) {
    buf2[pos++] = '0' + (speed / 100) % 10;
    buf2[pos++] = '0' + (speed / 10) % 10;
    buf2[pos++] = '0' + speed % 10;
  } else if (speed >= 10) {
    buf2[pos++] = '0' + (speed / 10) % 10;
    buf2[pos++] = '0' + speed % 10;
  } else {
    buf2[pos++] = '0' + speed;
  }

  buf2[pos] = '\0';

  // Copy to display command
  strcpy(out->display.line1, buf1);
  strcpy(out->display.line2, buf2);
}

void blfm_controller_process_bigsound(const blfm_bigsound_event_t *event,
                                      blfm_actuator_command_t *out) {
  if (!event || !out)
    return;
  
 //   blfm_gpio_config_output((uint32_t)GPIOB, 11);
  blfm_gpio_set_pin((uint32_t)GPIOB, 11);

}
