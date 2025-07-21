/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "blfm_config.h"
#if BLFM_ENABLED_OLED

#include "blfm_oled.h"
#include "blfm_i2c1.h"
#include "blfm_font8x8.h"
#include "libc_stubs.h"

#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define OLED_PAGES 4

// Simple framebuffer - one byte per column per page
static uint8_t framebuffer[OLED_PAGES][OLED_WIDTH];
static bool initialized = false;

// Send command to OLED
static void send_cmd(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};
    blfm_i2c1_write(OLED_ADDR, data, 2);
}

// Send data to OLED
static void send_data(uint8_t data) {
    uint8_t buffer[2] = {0x40, data};
    blfm_i2c1_write(OLED_ADDR, buffer, 2);
}

void blfm_oled_init(void) {
    // Very basic SSD1306 initialization
    send_cmd(0xAE); // Display off
    send_cmd(0xD5); // Set display clock
    send_cmd(0x80); // Default ratio
    send_cmd(0xA8); // Set multiplex
    send_cmd(0x1F); // 32 pixels height
    send_cmd(0xD3); // Set display offset
    send_cmd(0x00); // No offset
    send_cmd(0x40); // Set start line
    send_cmd(0x8D); // Charge pump
    send_cmd(0x14); // Enable charge pump
    send_cmd(0x20); // Memory mode
    send_cmd(0x00); // Horizontal addressing
    send_cmd(0xA1); // Segment remap
    send_cmd(0xC8); // COM scan direction
    send_cmd(0xDA); // COM pins
    send_cmd(0x02); // Sequential COM pins
    send_cmd(0x81); // Set contrast
    send_cmd(0x8F); // Medium contrast
    send_cmd(0xD9); // Set precharge
    send_cmd(0xF1); // Default precharge
    send_cmd(0xDB); // Set VCOM detect
    send_cmd(0x40); // Default VCOM detect
    send_cmd(0xA4); // Resume to RAM content
    send_cmd(0xA6); // Normal display
    send_cmd(0xAF); // Display on
    
    initialized = true;
    blfm_oled_clear();
}

void blfm_oled_clear(void) {
    if (!initialized) return;
    
    // Clear framebuffer
    for (int page = 0; page < OLED_PAGES; page++) {
        for (int x = 0; x < OLED_WIDTH; x++) {
            framebuffer[page][x] = 0x00;
        }
    }
}

void blfm_oled_flush(void) {
    if (!initialized) return;
    
    for (int page = 0; page < OLED_PAGES; page++) {
        // Set page address
        send_cmd(0xB0 | page);  // Page start address
        send_cmd(0x00);         // Lower column start address
        send_cmd(0x10);         // Higher column start address
        
        // Send page data
        for (int x = 0; x < OLED_WIDTH; x++) {
            send_data(framebuffer[page][x]);
        }
    }
}

void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    if (color) {
        framebuffer[page][x] |= (1 << bit);
    } else {
        framebuffer[page][x] &= ~(1 << bit);
    }
}

// Simple character drawing
void blfm_oled_draw_char(uint8_t x, uint8_t page, char c) {
    if (x >= OLED_WIDTH || page >= OLED_PAGES) return;
    if (c < 32 || c > 127) c = '?';
    
    const uint8_t *font_data = blfm_font8x8_basic[(uint8_t)c];
    
    // Draw character column by column
    for (int col = 0; col < 8 && (x + col) < OLED_WIDTH; col++) {
        uint8_t column = 0;
        for (int row = 0; row < 8; row++) {
            if (font_data[row] & (1 << col)) {
                column |= (1 << row);
            }
        }
        framebuffer[page][x + col] = column;
    }
}

void blfm_oled_draw_text(uint8_t x, uint8_t page, const char *str) {
    if (!str) return;
    
    while (*str && x < OLED_WIDTH) {
        blfm_oled_draw_char(x, page, *str);
        x += 8;
        str++;
    }
}

// Ultra-simple apply function
void blfm_oled_apply(const blfm_oled_command_t *data) {
    if (!data || !initialized) return;
    
    blfm_oled_clear();
    
    // Draw big text in center
    if (data->bigtext[0] != '\0') {
        int len = strlen(data->bigtext);
        int start_x = (OLED_WIDTH - len * 8) / 2;
        if (start_x < 0) start_x = 0;
        blfm_oled_draw_text(start_x, 1, data->bigtext);
    }
    
    // Draw small text at top
    if (data->smalltext1[0] != '\0') {
        blfm_oled_draw_text(0, 0, data->smalltext1);
    }
    
    // Draw small text at bottom
    if (data->smalltext2[0] != '\0') {
        blfm_oled_draw_text(0, 3, data->smalltext2);
    }
    
    // Simple progress bar at bottom
    if (data->progress_percent <= 100) {
        int filled = (data->progress_percent * OLED_WIDTH) / 100;
        for (int x = 0; x < OLED_WIDTH; x++) {
            framebuffer[3][x] |= (x < filled) ? 0x80 : 0x40; // Top bit for filled, second bit for empty
        }
    }
    
    blfm_oled_flush();
}

// Stub functions for compatibility
void blfm_oled_draw_line(int x0, int y0, int x1, int y1) { (void)x0; (void)y0; (void)x1; (void)y1; }
void blfm_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) { (void)x; (void)y; (void)w; (void)h; }
void blfm_oled_invert(void) { if (initialized) send_cmd(0xA7); }
void blfm_oled_draw_progress_bar(uint8_t percent) { (void)percent; }
void blfm_oled_scroll_text(const char *text, uint8_t speed_ms) { (void)text; (void)speed_ms; }
void blfm_oled_scroll_horizontal(const char *text, uint8_t speed) { (void)text; (void)speed; }
void blfm_oled_blink(uint8_t times, uint16_t delay_ms) { (void)times; (void)delay_ms; }

#endif /* BLFM_ENABLED_OLED */