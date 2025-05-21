/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_display.h"
#include "blfm_i2c.h"
#include "FreeRTOS.h"
#include "task.h"

#define LCD_ADDR 0x27 // Adjust if needed

#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define READ_WRITE 0x02
#define REGISTER_SELECT 0x01

static void lcd_send_nibble(uint8_t nibble);
static void lcd_write_byte(uint8_t value, uint8_t mode);
static void lcd_send_command(uint8_t cmd);
static void lcd_send_data(uint8_t data);
static void lcd_delay(void);

void blfm_display_init(void) {
  vTaskDelay(pdMS_TO_TICKS(50)); // LCD power-up delay

  lcd_send_command(0x33); // Init sequence
  lcd_send_command(0x32); // 4-bit mode
  lcd_send_command(0x28); // 2 line, 5x8 font
  lcd_send_command(0x0C); // Display ON, cursor OFF
  lcd_send_command(0x06); // Entry mode set
  lcd_send_command(0x01); // Clear display
  vTaskDelay(pdMS_TO_TICKS(5));
}

void blfm_display_test_message(void) {
  const char *line1 = "Belfhym Ready!";
  const char *line2 = "Lunar System OK";

  lcd_send_command(0x80); // Line 1
  while (*line1) {
    lcd_send_data(*line1++);
  }

  lcd_send_command(0xC0); // Line 2
  while (*line2) {
    lcd_send_data(*line2++);
  }
}

// === Low-Level Helpers ===

static void lcd_write_byte(uint8_t value, uint8_t mode) {
  uint8_t high = (value & 0xF0) | mode | LCD_BACKLIGHT;
  uint8_t low  = ((value << 4) & 0xF0) | mode | LCD_BACKLIGHT;

  lcd_send_nibble(high);
  lcd_send_nibble(low);
}

static void lcd_send_nibble(uint8_t nibble) {
  blfm_i2c_write_byte_simple(LCD_ADDR, nibble | ENABLE);
  lcd_delay();
  blfm_i2c_write_byte_simple(LCD_ADDR, nibble & ~ENABLE);
  lcd_delay();
}

static void lcd_send_command(uint8_t cmd) {
  lcd_write_byte(cmd, 0);
}

static void lcd_send_data(uint8_t data) {
  lcd_write_byte(data, REGISTER_SELECT);
}

static void lcd_delay(void) {
  vTaskDelay(pdMS_TO_TICKS(1)); // ~1ms delay for enable pulse
}
