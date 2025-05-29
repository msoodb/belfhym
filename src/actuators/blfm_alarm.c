
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_alarm.h"

void blfm_alarm_init(void) {
#if BLFM_ALARM_DISABLED
  return;
#endif
  
}

void blfm_alarm_apply(const blfm_alarm_command_t *cmd) {
#if BLFM_ALARM_DISABLED
  return;
#endif

  if (!cmd || !cmd->active) {
    // Turn alarm off
    // e.g., hardware_stop_alarm();
    return;
  }

  // Implement different alarm patterns based on pattern_id, duration, volume
  // Example placeholder:
  switch (cmd->pattern_id) {
    case 1: // short beep
      // hardware_beep_short(cmd->volume);
      break;
    case 2: // long beep
      // hardware_beep_long(cmd->volume);
      break;
    case 3: // continuous beep
      // hardware_beep_continuous(cmd->volume, cmd->duration_ms);
      break;
    default:
      // default beep or no action
      break;
  }
}
