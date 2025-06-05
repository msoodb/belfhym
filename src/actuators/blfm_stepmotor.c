
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_stepmotor.h"
#include "blfm_gpio.h" // For pin control (assumed)
#include "blfm_config.h"

// === Placeholder motor pin map ===
// You should define proper step/dir/en pins per motor
typedef struct {
  uint32_t step_port;
  uint8_t step_pin;
  uint32_t dir_port;
  uint8_t dir_pin;
  uint32_t en_port;
  uint8_t en_pin;
} blfm_stepmotor_hw_t;

static const blfm_stepmotor_hw_t hw_map[BLFM_STEPMOTOR_COUNT] = {
  [BLFM_STEPMOTOR_NECK]  = {GPIOA_BASE, 0, GPIOA_BASE, 1, GPIOA_BASE, 2},
  [BLFM_STEPMOTOR_ELBOW] = {GPIOB_BASE, 3, GPIOB_BASE, 4, GPIOB_BASE, 5},
  [BLFM_STEPMOTOR_WRIST] = {GPIOC_BASE, 6, GPIOC_BASE, 7, GPIOC_BASE, 8}
};

void blfm_stepmotor_init(void) {
  for (int i = 0; i < BLFM_STEPMOTOR_COUNT; ++i) {
    blfm_gpio_config_output(hw_map[i].step_port, hw_map[i].step_pin);
    blfm_gpio_config_output(hw_map[i].dir_port, hw_map[i].dir_pin);
    blfm_gpio_config_output(hw_map[i].en_port, hw_map[i].en_pin);
    blfm_gpio_clear_pin(hw_map[i].en_port, hw_map[i].en_pin); // Enable motor
  }
}

void blfm_stepmotor_apply(blfm_stepmotor_id_t id, const blfm_stepmotor_command_t *cmd) {
  if (!cmd || id >= BLFM_STEPMOTOR_COUNT)
    return;

  const blfm_stepmotor_hw_t *hw = &hw_map[id];

  // Enable/disable
  if (cmd->enabled)
    blfm_gpio_clear_pin(hw->en_port, hw->en_pin); // Active-low
  else
    blfm_gpio_set_pin(hw->en_port, hw->en_pin);

  // Set direction
  if (cmd->target_position >= 0)
    blfm_gpio_set_pin(hw->dir_port, hw->dir_pin);
  else
    blfm_gpio_clear_pin(hw->dir_port, hw->dir_pin);

  // TODO: Replace with actual step generation code using timers or stepping library
  // Here we mock N steps at dummy timing
  uint32_t steps = (cmd->target_position >= 0) ? cmd->target_position : -cmd->target_position;
  for (uint32_t i = 0; i < steps; ++i) {
    blfm_gpio_set_pin(hw->step_port, hw->step_pin);
    // blfm_delay_us(1000000 / cmd->speed / 2); // half period
    blfm_gpio_clear_pin(hw->step_port, hw->step_pin);
    // blfm_delay_us(1000000 / cmd->speed / 2);
  }
}

void blfm_stepmotor_apply_all(const blfm_stepmotor_command_t cmds[BLFM_STEPMOTOR_COUNT]) {
  for (int i = 0; i < BLFM_STEPMOTOR_COUNT; ++i) {
    blfm_stepmotor_apply((blfm_stepmotor_id_t)i, &cmds[i]);
  }
}
