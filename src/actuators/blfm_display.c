/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */
 
#include "blfm_display.h"
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "blfm_gpio.h"

#define LCD_GPIO GPIOA

#define LCD_RS_PIN 0  // PA0
#define LCD_E_PIN  1  // PA1
#define LCD_D4_PIN 2  // PA2
#define LCD_D5_PIN 3  // PA3
#define LCD_D6_PIN 4  // PA4
#define LCD_D7_PIN 5  // PA5

static void lcd_pulse_enable(void);
static void lcd_write_nibble(uint8_t nibble);
//static void lcd_send_command(uint8_t cmd);
//static void lcd_send_data(uint8_t data);

void blfm_display_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_RS_PIN);
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_E_PIN);
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D4_PIN);
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D5_PIN);
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D6_PIN);
    blfm_gpio_config_output((uint32_t)LCD_GPIO, LCD_D7_PIN);
}

void blfm_display_startup_sequence(void) {
    vTaskDelay(pdMS_TO_TICKS(50)); // wait after power-on

    // Initial 4-bit init sequence (write upper nibble only)
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(1));
    lcd_write_nibble(0x02); vTaskDelay(pdMS_TO_TICKS(1)); // switch to 4-bit mode

    // Now use full commands (2 nibbles)
    blfm_lcd_send_command(0x28); // 4-bit, 2-line, 5x8 font
    blfm_lcd_send_command(0x0C); // display ON, cursor OFF
    blfm_lcd_send_command(0x06); // entry mode set
    blfm_lcd_send_command(0x01); // clear display
    vTaskDelay(pdMS_TO_TICKS(2));

    blfm_lcd_send_command(0x01); // clear
    blfm_lcd_send_data('A');     // test display
}

static void lcd_pulse_enable(void) {
    LCD_GPIO->BSRR = (1 << LCD_E_PIN);   // E high
    // Small delay — this is critical. Use a short busy loop
    for (volatile int i = 0; i < 30; i++) __asm__("nop");
    LCD_GPIO->BRR = (1 << LCD_E_PIN);    // E low
    for (volatile int i = 0; i < 30; i++) __asm__("nop");
}

static void lcd_write_nibble(uint8_t nibble) {
    LCD_GPIO->BRR = (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);
    if (nibble & 0x01) LCD_GPIO->BSRR = (1 << LCD_D4_PIN);
    if (nibble & 0x02) LCD_GPIO->BSRR = (1 << LCD_D5_PIN);
    if (nibble & 0x04) LCD_GPIO->BSRR = (1 << LCD_D6_PIN);
    if (nibble & 0x08) LCD_GPIO->BSRR = (1 << LCD_D7_PIN);
    lcd_pulse_enable();
}

void blfm_lcd_send_command(uint8_t cmd) {
    LCD_GPIO->BRR = (1 << LCD_RS_PIN); // RS = 0
    lcd_write_nibble(cmd >> 4);
    lcd_write_nibble(cmd & 0x0F);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void blfm_lcd_send_data(uint8_t data) {
    LCD_GPIO->BSRR = (1 << LCD_RS_PIN); // RS = 1
    lcd_write_nibble(data >> 4);
    lcd_write_nibble(data & 0x0F);
    vTaskDelay(pdMS_TO_TICKS(2));
}
