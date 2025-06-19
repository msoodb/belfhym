
#ifndef BLFM_STATE_H
#define BLFM_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  BLFM_MODE_AUTO = 0,
  BLFM_MODE_MANUAL,
  BLFM_MODE_EMERGENCY
} blfm_mode_t;

typedef struct {
  blfm_mode_t current_mode;
  bool overheat;
  bool obstacle_near;
  bool manual_override;
  uint32_t last_mode_change_time;
} blfm_system_state_t;

#endif // BLFM_STATE_H
