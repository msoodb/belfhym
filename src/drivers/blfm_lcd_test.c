#include "stm32f1xx.h"
#include <stdint.h>

// === Pin Definitions ===
#define LCD_GPIO GPIOA

#define LCD_RS_PIN 0  // PA0
#define LCD_E_PIN  1  // PA1
#define LCD_D4_PIN 2  // PA2
#define LCD_D5_PIN 3  // PA3
#define LCD_D6_PIN 4  // PA4
#define LCD_D7_PIN 5  // PA5

static void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 8000; i++) __NOP();
}

static void lcd_pulse_enable(void) {
    LCD_GPIO->BSRR = (1 << LCD_E_PIN); // E = High
    delay_ms(1);
    LCD_GPIO->BRR = (1 << LCD_E_PIN);  // E = Low
    delay_ms(1);
}

static void lcd_write_nibble(uint8_t nibble) {
    LCD_GPIO->BRR = (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) |
                    (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

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
    delay_ms(2);
}

static void lcd_send_data(uint8_t data) {
    LCD_GPIO->BSRR = (1 << LCD_RS_PIN); // RS = 1
    lcd_write_nibble(data >> 4);
    lcd_write_nibble(data & 0x0F);
    delay_ms(2);
}

void blfm_lcd_basic_test(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // Configure PA0-PA5 as output
    for (int pin=0; pin<=5; pin++) {
        LCD_GPIO->CRL &= ~(0xF << (pin*4));
        LCD_GPIO->CRL |=  (0x2 << (pin*4));
    }
    while (1) {
        LCD_GPIO->BSRR = (1 << LCD_E_PIN);  // E high
        delay_ms(500);
        LCD_GPIO->BRR = (1 << LCD_E_PIN);   // E low
        delay_ms(500);
    }
}

void blfm_lcd_test_simple(void) {
    // Enable GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA0–PA5 as push-pull outputs, 2 MHz
    for (int pin = 0; pin <= 5; pin++) {
        if (pin <= 3) {
            LCD_GPIO->CRL &= ~(0xF << (pin * 4));
            LCD_GPIO->CRL |=  (0x2 << (pin * 4)); // Output mode, 2 MHz
        } else {
            int shift = (pin - 4) * 4;
            LCD_GPIO->CRL &= ~(0xF << shift);
            LCD_GPIO->CRL |=  (0x2 << shift); // Output mode, 2 MHz
        }
    }

    delay_ms(50); // Power-on delay

    // LCD init sequence (4-bit mode)
    lcd_write_nibble(0x03); delay_ms(5);
    lcd_write_nibble(0x03); delay_ms(5);
    lcd_write_nibble(0x03); delay_ms(5);
    lcd_write_nibble(0x02); delay_ms(5);

    lcd_send_command(0x28); // Function set: 4-bit, 2 lines
    lcd_send_command(0x0C); // Display ON, Cursor OFF
    lcd_send_command(0x06); // Entry mode set
    lcd_send_command(0x01); // Clear display
    delay_ms(5);

    // Write Line 1
    lcd_send_command(0x80);
    const char *line1 = "Hello Belfhym";
    while (*line1) lcd_send_data(*line1++);

    // Write Line 2
    lcd_send_command(0xC0);
    const char *line2 = "LCD OK";
    while (*line2) lcd_send_data(*line2++);
}
