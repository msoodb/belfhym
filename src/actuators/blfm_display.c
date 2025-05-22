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

#define LCD_GPIO GPIOA

#define LCD_RS_PIN 0  // PA0
#define LCD_E_PIN  1  // PA1
#define LCD_D4_PIN 2  // PA2
#define LCD_D5_PIN 3  // PA3
#define LCD_D6_PIN 4  // PA4
#define LCD_D7_PIN 5  // PA5

static void lcd_pulse_enable(void);
static void lcd_write_nibble(uint8_t nibble);
static void lcd_send_command(uint8_t cmd);
static void lcd_send_data(uint8_t data);

void blfm_display_init(void) {
    // Enable GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA0–PA5 as output push-pull, 2 MHz
    for (int pin = 0; pin <= 5; pin++) {
        LCD_GPIO->CRL &= ~(0xF << (pin * 4));
        LCD_GPIO->CRL |=  (0x2 << (pin * 4)); // Output, push-pull, 2 MHz
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for power-up

    // 4-bit init sequence
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write_nibble(0x03); vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write_nibble(0x02); vTaskDelay(pdMS_TO_TICKS(5)); // 4-bit mode

    lcd_send_command(0x28); // 4-bit, 2-line, 5x8
    lcd_send_command(0x0C); // Display ON, cursor OFF
    lcd_send_command(0x06); // Entry mode
    lcd_send_command(0x01); // Clear
    vTaskDelay(pdMS_TO_TICKS(5));
}

void blfm_display_test_message(void) {
    const char *line1 = "Belfhym Ready!";
    const char *line2 = "Lunar System OK";

    lcd_send_command(0x80); // Line 1
    while (*line1) lcd_send_data(*line1++);

    lcd_send_command(0xC0); // Line 2
    while (*line2) lcd_send_data(*line2++);
}

// === Low-level GPIO functions ===

static void lcd_pulse_enable(void) {
    LCD_GPIO->BSRR = (1 << LCD_E_PIN); // E high
    vTaskDelay(pdMS_TO_TICKS(1));
    LCD_GPIO->BRR = (1 << LCD_E_PIN);  // E low
    vTaskDelay(pdMS_TO_TICKS(1));
}

static void lcd_write_nibble(uint8_t nibble) {
    // Clear D4–D7
    LCD_GPIO->BRR = (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

    // Set new values
    if (nibble & 0x01) LCD_GPIO->BSRR = (1 << LCD_D4_PIN);
    if (nibble & 0x02) LCD_GPIO->BSRR = (1 << LCD_D5_PIN);
    if (nibble & 0x04) LCD_GPIO->BSRR = (1 << LCD_D6_PIN);
    if (nibble & 0x08) LCD_GPIO->BSRR = (1 << LCD_D7_PIN);

    lcd_pulse_enable();
}

static void lcd_send_command(uint8_t cmd) {
    LCD_GPIO->BRR = (1 << LCD_RS_PIN); // RS = 0
    lcd_write_nibble(cmd >> 4);
    lcd_write_nibble(cmd & 0x0F);
}

static void lcd_send_data(uint8_t data) {
    LCD_GPIO->BSRR = (1 << LCD_RS_PIN); // RS = 1
    lcd_write_nibble(data >> 4);
    lcd_write_nibble(data & 0x0F);
}
