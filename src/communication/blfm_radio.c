
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_radio.h"

void blfm_radio_init(void) {
#if BLFM_RADIO_DISABLED
  return;
#endif

    // TODO: Implement blfm_radio
}

void blfm_radio_apply(const blfm_radio_command_t *cmd){
#if BLFM_RADIO_DISABLED
  return;
#endif

  if (!cmd ) return;
  return;
}

