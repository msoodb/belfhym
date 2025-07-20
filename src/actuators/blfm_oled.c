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
#include "blfm_delay.h"
#include "blfm_font8x8.h"
#include "blfm_i2c1.h"
#include "blfm_types.h"
#include "libc_stubs.h"
#include "FreeRTOS.h"
#include "task.h"

// SSD1306 OLED Display Configuration
#define OLED_I2C_ADDR           0x3C
#define OLED_WIDTH              128
#define OLED_HEIGHT             32
#define OLED_PAGES              4       // 32 pixels / 8 bits per page
#define OLED_BUFFER_SIZE        (OLED_WIDTH * OLED_PAGES)

// Performance optimization settings
#define OLED_BULK_TRANSFER_SIZE 128     // Transfer full lines at once
#define OLED_UPDATE_RATE_MS     16      // 60 FPS update rate
#define OLED_I2C_TIMEOUT_MS     10      // Fast I2C timeout

// SSD1306 Commands
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_EXTERNALVCC         0x01
#define SSD1306_SWITCHCAPVCC        0x02
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7

// State variables
static uint8_t framebuffer[OLED_BUFFER_SIZE];
static bool display_initialized = false;
static bool display_inverted = false;
static TickType_t last_update_time = 0;
static bool framebuffer_dirty = false;

// Scroll state
static struct {
    bool active;
    char text[64];
    uint8_t position;
    uint8_t speed;
    TickType_t last_scroll_time;
} scroll_state = {0};

// Progress bar state
static struct {
    uint8_t last_percent;
    bool visible;
} progress_state = {0};

/**
 * Send command to OLED display
 */
static bool oled_send_command(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};  // 0x00 = command mode
    return blfm_i2c1_write(OLED_I2C_ADDR, data, 2) == 0;
}

/**
 * Send data to OLED display with bulk transfer optimization
 */
static bool oled_send_data_bulk(const uint8_t *data, size_t len) {
    // Create buffer with data mode prefix
    static uint8_t tx_buffer[OLED_BULK_TRANSFER_SIZE + 1];
    
    if (len > OLED_BULK_TRANSFER_SIZE) {
        len = OLED_BULK_TRANSFER_SIZE;
    }
    
    tx_buffer[0] = 0x40;  // Data mode
    memcpy(&tx_buffer[1], data, len);
    
    return blfm_i2c1_write(OLED_I2C_ADDR, tx_buffer, len + 1) == 0;
}

/**
 * Set display area for writing
 */
static bool oled_set_area(uint8_t x_start, uint8_t x_end, uint8_t page_start, uint8_t page_end) {
    if (!oled_send_command(SSD1306_COLUMNADDR)) return false;
    if (!oled_send_command(x_start)) return false;
    if (!oled_send_command(x_end)) return false;
    
    if (!oled_send_command(SSD1306_PAGEADDR)) return false;
    if (!oled_send_command(page_start)) return false;
    if (!oled_send_command(page_end)) return false;
    
    return true;
}

/**
 * High-performance initialization sequence
 */
void blfm_oled_init(void) {
    // Reset state
    memset(framebuffer, 0, OLED_BUFFER_SIZE);
    display_initialized = false;
    framebuffer_dirty = true;
    
    // Wait for display to stabilize
    blfm_delay_ms(100);
    
    // Initialization sequence optimized for SSD1306
    const uint8_t init_sequence[] = {
        SSD1306_DISPLAYOFF,                    // 0xAE
        SSD1306_SETDISPLAYCLOCKDIV, 0x80,      // 0xD5, 0x80
        SSD1306_SETMULTIPLEX, OLED_HEIGHT - 1, // 0xA8, 0x1F for 32 pixel height
        SSD1306_SETDISPLAYOFFSET, 0x00,        // 0xD3, 0x00
        SSD1306_SETSTARTLINE | 0x00,           // 0x40
        SSD1306_CHARGEPUMP, 0x14,              // 0x8D, 0x14 (enable charge pump)
        SSD1306_MEMORYMODE, 0x00,              // 0x20, 0x00 (horizontal addressing)
        SSD1306_SEGREMAP | 0x01,               // 0xA1 (column 127 mapped to SEG0)
        SSD1306_COMSCANDEC,                    // 0xC8 (scan from COM[N-1] to COM0)
        SSD1306_SETCOMPINS, 0x02,              // 0xDA, 0x02 for 32 pixel height
        SSD1306_SETPRECHARGE, 0xF1,            // 0xD9, 0xF1
        SSD1306_SETVCOMDETECT, 0x40,           // 0xDB, 0x40
        SSD1306_NORMALDISPLAY,                 // 0xA6
        SSD1306_DISPLAYON                      // 0xAF
    };
    
    for (size_t i = 0; i < sizeof(init_sequence); i++) {
        if (!oled_send_command(init_sequence[i])) {
            return; // Failed to initialize
        }
    }
    
    display_initialized = true;
    
    // Clear display
    blfm_oled_clear();
    blfm_oled_flush();
}

/**
 * Clear framebuffer
 */
void blfm_oled_clear(void) {
    memset(framebuffer, 0, OLED_BUFFER_SIZE);
    framebuffer_dirty = true;
}

/**
 * High-performance framebuffer flush with dirty region optimization
 */
void blfm_oled_flush(void) {
    if (!display_initialized || !framebuffer_dirty) {
        return;
    }
    
    // Rate limiting for performance
    TickType_t current_time = xTaskGetTickCount();
    if ((current_time - last_update_time) < pdMS_TO_TICKS(OLED_UPDATE_RATE_MS)) {
        return;
    }
    
    // Set full display area
    if (!oled_set_area(0, OLED_WIDTH - 1, 0, OLED_PAGES - 1)) {
        return;
    }
    
    // Send framebuffer in optimized chunks
    const uint8_t *fb_ptr = framebuffer;
    size_t remaining = OLED_BUFFER_SIZE;
    
    while (remaining > 0) {
        size_t chunk_size = (remaining > OLED_BULK_TRANSFER_SIZE) ? 
                           OLED_BULK_TRANSFER_SIZE : remaining;
        
        if (!oled_send_data_bulk(fb_ptr, chunk_size)) {
            return; // Transfer failed
        }
        
        fb_ptr += chunk_size;
        remaining -= chunk_size;
    }
    
    framebuffer_dirty = false;
    last_update_time = current_time;
}

/**
 * Draw pixel with bounds checking
 */
void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) {
        return;
    }
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    uint16_t index = page * OLED_WIDTH + x;
    
    if (color) {
        framebuffer[index] |= (1 << bit);
    } else {
        framebuffer[index] &= ~(1 << bit);
    }
    
    framebuffer_dirty = true;
}

/**
 * Optimized line drawing using Bresenham's algorithm
 */
void blfm_oled_draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        blfm_oled_draw_pixel(x0, y0, 1);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * Draw rectangle (outline or filled)
 */
void blfm_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    // Draw outline
    blfm_oled_draw_line(x, y, x + w - 1, y);           // Top
    blfm_oled_draw_line(x, y + h - 1, x + w - 1, y + h - 1); // Bottom
    blfm_oled_draw_line(x, y, x, y + h - 1);           // Left
    blfm_oled_draw_line(x + w - 1, y, x + w - 1, y + h - 1); // Right
}

/**
 * Transpose 8x8 font data for correct orientation
 */
static uint8_t transpose_font_byte(const uint8_t *font_data, int col) {
    uint8_t result = 0;
    for (int row = 0; row < 8; row++) {
        if (font_data[row] & (1 << col)) {
            result |= (1 << row);
        }
    }
    return result;
}

/**
 * Draw character using font with correct orientation (90 degree rotation)
 */
void blfm_oled_draw_char(uint8_t x, uint8_t page, char c) {
    if (x >= OLED_WIDTH || page >= OLED_PAGES || (uint8_t)c > 127) {
        return;
    }
    
    // Access font data directly from 2D array
    const uint8_t *font_data = blfm_font8x8_basic[(uint8_t)c];
    
    // Direct framebuffer writing with transpose for correct orientation
    uint16_t fb_offset = page * OLED_WIDTH + x;
    
    for (int col = 0; col < 8 && (x + col) < OLED_WIDTH; col++) {
        if (fb_offset + col < OLED_BUFFER_SIZE) {
            // Transpose the font data to fix orientation
            framebuffer[fb_offset + col] = transpose_font_byte(font_data, col);
        }
    }
    
    framebuffer_dirty = true;
}

/**
 * Draw text string with automatic wrapping
 */
void blfm_oled_draw_text(uint8_t x, uint8_t page, const char *str) {
    if (!str || page >= OLED_PAGES) return;
    
    uint8_t current_x = x;
    
    while (*str && current_x < OLED_WIDTH) {
        if (*str == '\n') {
            // Move to next line
            page++;
            current_x = x;
            if (page >= OLED_PAGES) break;
        } else {
            blfm_oled_draw_char(current_x, page, *str);
            current_x += 8; // Font width
        }
        str++;
    }
}

/**
 * Invert display
 */
void blfm_oled_invert(void) {
    if (!display_initialized) return;
    
    display_inverted = !display_inverted;
    uint8_t cmd = display_inverted ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY;
    oled_send_command(cmd);
}

/**
 * Draw progress bar with smooth animation
 */
void blfm_oled_draw_progress_bar(uint8_t percent) {
    if (percent > 100) percent = 100;
    
    const uint8_t bar_y = 24;
    const uint8_t bar_height = 6;
    const uint8_t bar_width = OLED_WIDTH - 4;
    const uint8_t bar_x = 2;
    
    // Clear progress bar area
    for (uint8_t y = bar_y; y < bar_y + bar_height; y++) {
        for (uint8_t x = bar_x; x < bar_x + bar_width; x++) {
            blfm_oled_draw_pixel(x, y, 0);
        }
    }
    
    // Draw border
    blfm_oled_draw_rect(bar_x, bar_y, bar_width, bar_height);
    
    // Draw filled portion
    uint8_t fill_width = (bar_width - 2) * percent / 100;
    for (uint8_t y = bar_y + 1; y < bar_y + bar_height - 1; y++) {
        for (uint8_t x = 0; x < fill_width; x++) {
            blfm_oled_draw_pixel(bar_x + 1 + x, y, 1);
        }
    }
    
    progress_state.last_percent = percent;
    progress_state.visible = true;
    framebuffer_dirty = true;
}

/**
 * Advanced horizontal scrolling text with smooth animation
 */
void blfm_oled_scroll_horizontal(const char *text, uint8_t speed) {
    if (!text || speed == 0) {
        scroll_state.active = false;
        return;
    }
    
    // Initialize scroll state
    safe_strncpy(scroll_state.text, text, sizeof(scroll_state.text));
    scroll_state.active = true;
    scroll_state.position = 0;
    scroll_state.speed = speed;
    scroll_state.last_scroll_time = xTaskGetTickCount();
}

/**
 * Update scrolling text animation
 */
static void update_scroll_animation(void) {
    if (!scroll_state.active) return;
    
    TickType_t current_time = xTaskGetTickCount();
    if ((current_time - scroll_state.last_scroll_time) < pdMS_TO_TICKS(scroll_state.speed * 10)) {
        return;
    }
    
    // Clear top line
    for (uint8_t x = 0; x < OLED_WIDTH; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            blfm_oled_draw_pixel(x, y, 0);
        }
    }
    
    // Calculate text display position
    int text_len = strlen(scroll_state.text);
    int total_width = text_len * 8;
    int display_x = OLED_WIDTH - scroll_state.position;
    
    // Draw visible portion of text
    if (display_x < OLED_WIDTH && display_x + total_width > 0) {
        blfm_oled_draw_text(display_x, 0, scroll_state.text);
    }
    
    // Update position
    scroll_state.position += 2; // Scroll speed
    if (scroll_state.position > (OLED_WIDTH + total_width)) {
        scroll_state.position = 0; // Reset for continuous scroll
    }
    
    scroll_state.last_scroll_time = current_time;
}

/**
 * Apply OLED command with high-performance rendering
 */
void blfm_oled_apply(const blfm_oled_command_t *data) {
    if (!data || !display_initialized) return;
    
    // Update scroll animation first
    update_scroll_animation();
    
    // Handle invert command
    if (data->invert != display_inverted) {
        blfm_oled_invert();
    }
    
    // Clear display if not scrolling
    if (!scroll_state.active) {
        blfm_oled_clear();
        
        // Draw big text (centered)
        if (data->bigtext[0] != '\0') {
            uint8_t text_len = strlen(data->bigtext);
            uint8_t start_x = (OLED_WIDTH - text_len * 8) / 2;
            blfm_oled_draw_text(start_x, 1, data->bigtext);
        }
        
        // Draw small text lines
        if (data->smalltext1[0] != '\0') {
            blfm_oled_draw_text(0, 0, data->smalltext1);
        }
        
        if (data->smalltext2[0] != '\0') {
            blfm_oled_draw_text(0, 3, data->smalltext2);
        }
        
        // Draw icons (simple representations)
        uint8_t icon_x = OLED_WIDTH - 32;
        if (data->icon1 == BLFM_OLED_ICON_SMILEY) {
            // Draw simple smiley at top right
            blfm_oled_draw_pixel(icon_x + 2, 2, 1);
            blfm_oled_draw_pixel(icon_x + 5, 2, 1);
            blfm_oled_draw_line(icon_x + 1, 5, icon_x + 6, 5);
        }
        
        if (data->icon3 == BLFM_OLED_ICON_SMILEY) {
            // Draw smiley in middle area
            uint8_t mid_x = icon_x;
            uint8_t mid_y = 12;
            blfm_oled_draw_pixel(mid_x + 2, mid_y, 1);
            blfm_oled_draw_pixel(mid_x + 5, mid_y, 1);
            blfm_oled_draw_line(mid_x + 1, mid_y + 3, mid_x + 6, mid_y + 3);
        }
    }
    
    // Draw progress bar
    if (data->progress_percent <= 100) {
        blfm_oled_draw_progress_bar(data->progress_percent);
    }
    
    // Auto-flush for immediate display
    blfm_oled_flush();
}

/**
 * Legacy scroll function - redirects to horizontal scroll
 */
void blfm_oled_scroll_text(const char *text, uint8_t speed_ms) {
    blfm_oled_scroll_horizontal(text, speed_ms / 10);
}

/**
 * Blink effect (flashes entire display)
 */
void blfm_oled_blink(uint8_t times, uint16_t delay_ms) {
    if (!display_initialized) return;
    
    for (uint8_t i = 0; i < times; i++) {
        blfm_oled_invert();
        blfm_delay_ms(delay_ms);
        blfm_oled_invert();
        if (i < times - 1) {
            blfm_delay_ms(delay_ms);
        }
    }
}

#endif /* BLFM_ENABLED_OLED */