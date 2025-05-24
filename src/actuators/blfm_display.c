/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 *
 * See LICENSE file for details.
 */

#include "blfm_display.h"
#include "FreeRTOS.h"
#include "blfm_gpio.h"
#include "stm32f1xx.h"
#include "task.h"

#define LCD_GPIO GPIOA

#define LCD_RS_PIN 0 // PA0
#define LCD_E_PIN 1  // PA1
#define LCD_D4_PIN 2 // PA2
#define LCD_D5_PIN 3 // PA3
#define LCD_D6_PIN 4 // PA4
#define LCD_D7_PIN 5 // PA5

static void lcd_pulse_enable(void);
static void lcd_write_nibble(uint8_t nibble);
static void lcd_busy_delay(void);
static void blfm_lcd_send_command(uint8_t cmd);
static void blfm_lcd_send_data(uint8_t data);
static void safe_delay_ms(uint32_t ms);

void blfm_display_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_RS_PIN);
  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_E_PIN);
  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D4_PIN);
  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D5_PIN);
  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D6_PIN);
  blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D7_PIN);

  // Manual delay for power-on wait (~50ms)
  for (volatile int i = 0; i < 50000; i++)
    __asm__("nop");

  lcd_write_nibble(0x03);
  lcd_busy_delay();
  lcd_write_nibble(0x03);
  lcd_busy_delay();
  lcd_write_nibble(0x03);
  lcd_busy_delay();
  lcd_write_nibble(0x02); // Switch to 4-bit mode
  lcd_busy_delay();

  blfm_lcd_send_command(0x28); // Function set: 4-bit, 2-line, 5x8 font
  blfm_lcd_send_command(0x0C); // Display ON, cursor OFF
  blfm_lcd_send_command(0x06); // Entry mode set: Increment cursor
  blfm_lcd_send_command(0x01); // Clear display
  lcd_busy_delay();            // Wait manually instead of vTaskDelay

  // Optional test output
  blfm_lcd_send_data('H');
  blfm_lcd_send_data('E');
  blfm_lcd_send_data('L');
  blfm_lcd_send_data('L');
  blfm_lcd_send_data('O');
}

void blfm_display_apply(const blfm_display_command_t *cmd) {
  if (!cmd)
    return;

  blfm_lcd_send_command(0x01); // Clear display
  vTaskDelay(pdMS_TO_TICKS(2));

  blfm_lcd_send_command(0x80); // Line 1 start
  for (const char *p = cmd->line1; *p; p++) {
    blfm_lcd_send_data((uint8_t)*p);
  }

  blfm_lcd_send_command(0xC0); // Line 2 start
  for (const char *p = cmd->line2; *p; p++) {
    blfm_lcd_send_data((uint8_t)*p);
  }
}

static void lcd_pulse_enable(void) {
  LCD_GPIO->BSRR = (1 << LCD_E_PIN); // E high
  for (volatile int i = 0; i < 200; i++)
    __asm__("nop");
  LCD_GPIO->BRR = (1 << LCD_E_PIN); // E low
  for (volatile int i = 0; i < 200; i++)
    __asm__("nop");
}

static void lcd_write_nibble(uint8_t nibble) {
  LCD_GPIO->BRR = (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) |
                  (1 << LCD_D7_PIN);

  if (nibble & 0x01)
    LCD_GPIO->BSRR = (1 << LCD_D4_PIN);
  if (nibble & 0x02)
    LCD_GPIO->BSRR = (1 << LCD_D5_PIN);
  if (nibble & 0x04)
    LCD_GPIO->BSRR = (1 << LCD_D6_PIN);
  if (nibble & 0x08)
    LCD_GPIO->BSRR = (1 << LCD_D7_PIN);

  lcd_pulse_enable();
}

static void blfm_lcd_send_command(uint8_t cmd) {
  LCD_GPIO->BRR = (1 << LCD_RS_PIN); // RS = 0 for command
  lcd_write_nibble(cmd >> 4);
  lcd_write_nibble(cmd & 0x0F);
  safe_delay_ms(2);
}

static void blfm_lcd_send_data(uint8_t data) {
  LCD_GPIO->BSRR = (1 << LCD_RS_PIN); // RS = 1 for data
  lcd_write_nibble(data >> 4);
  lcd_write_nibble(data & 0x0F);
  safe_delay_ms(2);
}

static void lcd_busy_delay(void) {
  for (volatile int i = 0; i < 10000; i++)
    __asm__("nop");
}

static void safe_delay_ms(uint32_t ms) {
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    vTaskDelay(pdMS_TO_TICKS(ms));
  } else {
    // Blocking busy wait if scheduler not running
    for (volatile uint32_t i = 0; i < ms * 8000; i++) {
      __asm__("nop");
    }
  }
}
