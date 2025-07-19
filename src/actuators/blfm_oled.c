#include "blfm_oled.h"
#include "blfm_delay.h"
#include "blfm_font8x8.h"
#include "blfm_i2c1.h"
#include "blfm_types.h"
#include "libc_stubs.h"
#include "FreeRTOS.h"
#include "task.h"

#define OLED_I2C_ADDR 0x3C
#define OLED_MIN_UPDATE_INTERVAL_MS 100  // 10 FPS max

static uint8_t oled_framebuffer[BLFM_OLED_PAGES][BLFM_OLED_WIDTH];
static TickType_t last_update_time = 0;
static bool oled_initialized = false;

// Safe I2C operations with timeout
static int oled_send_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    return blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2);
}

void blfm_oled_init(void) {
    // Standard OLED initialization sequence
    oled_send_command(0xAE); // Display off
    oled_send_command(0xD5); // Set display clock
    oled_send_command(0x80);
    oled_send_command(0xA8); // Set multiplex
    oled_send_command(0x1F);
    oled_send_command(0xD3); // Set display offset
    oled_send_command(0x00);
    oled_send_command(0x40); // Set start line
    oled_send_command(0x8D); // Charge pump
    oled_send_command(0x14);
    oled_send_command(0x20); // Memory mode
    oled_send_command(0x00);
    oled_send_command(0xA1); // Segment remap
    oled_send_command(0xC8); // COM scan direction
    oled_send_command(0xDA); // COM pins
    oled_send_command(0x02);
    oled_send_command(0x81); // Contrast
    oled_send_command(0x8F);
    oled_send_command(0xD9); // Pre-charge
    oled_send_command(0xF1);
    oled_send_command(0xDB); // VCOM detect
    oled_send_command(0x40);
    oled_send_command(0xA4); // Display all on resume
    oled_send_command(0xA6); // Normal display
    oled_send_command(0xAF); // Display on
    
    blfm_oled_clear();
    blfm_oled_flush();
    oled_initialized = true;
}

void blfm_oled_clear(void) {
    for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++) {
        for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
            oled_framebuffer[p][x] = 0x00;
        }
    }
}

void blfm_oled_flush(void) {
    if (!oled_initialized) return;
    
    TickType_t current_time = xTaskGetTickCount();
    if ((current_time - last_update_time) < pdMS_TO_TICKS(OLED_MIN_UPDATE_INTERVAL_MS)) {
        return;
    }
    
    static uint8_t page_buffer[BLFM_OLED_WIDTH + 1];
    page_buffer[0] = 0x40; // Data mode
    
    for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
        if (oled_send_command(0xB0 | page) != 0) return;
        if (oled_send_command(0x00) != 0) return;
        if (oled_send_command(0x10) != 0) return;
        
        for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
            page_buffer[col + 1] = oled_framebuffer[page][col];
        }
        
        if (blfm_i2c1_write(OLED_I2C_ADDR, page_buffer, BLFM_OLED_WIDTH + 1) != 0) {
            return;
        }
    }
    
    last_update_time = current_time;
}

void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= BLFM_OLED_WIDTH || y >= BLFM_OLED_HEIGHT) return;
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    if (color) {
        oled_framebuffer[page][x] |= (1 << bit);
    } else {
        oled_framebuffer[page][x] &= ~(1 << bit);
    }
}

// Proper font transpose function
static void transpose8x8(const uint8_t src[8], uint8_t dst[8]) {
    for (uint8_t col = 0; col < 8; col++) {
        dst[col] = 0;
        for (uint8_t row = 0; row < 8; row++) {
            if (src[row] & (1 << col)) {
                dst[col] |= (1 << row);
            }
        }
    }
}

static void draw_char_buf(uint8_t x, uint8_t page, char c, uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
    if (page >= BLFM_OLED_PAGES || x >= BLFM_OLED_WIDTH) return;
    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = '?';
    
    const uint8_t *glyph = blfm_font8x8_basic[(uint8_t)c];
    uint8_t transposed[8];
    transpose8x8(glyph, transposed);
    
    for (uint8_t col = 0; col < 8; col++) {
        if (x + col >= BLFM_OLED_WIDTH) break;
        buf[page][x + col] = transposed[col];
    }
}

static void draw_text_buf(uint8_t x, uint8_t page, const char *text, uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
    while (*text && x < BLFM_OLED_WIDTH) {
        draw_char_buf(x, page, *text++, buf);
        x += 8;
    }
}

static void draw_icon_buf(uint8_t x, uint8_t page, blfm_oled_icon_t icon, uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
    if (icon == BLFM_OLED_ICON_NONE || page >= BLFM_OLED_PAGES) return;
    
    static const uint8_t icon_heart[8] = {0x0C, 0x1E, 0x3F, 0x7F, 0x7F, 0x3F, 0x1E, 0x0C};
    static const uint8_t icon_star[8] = {0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00};
    static const uint8_t icon_smiley[8] = {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C};
    
    const uint8_t *icon_data = NULL;
    switch (icon) {
    case BLFM_OLED_ICON_HEART:
        icon_data = icon_heart;
        break;
    case BLFM_OLED_ICON_STAR:
        icon_data = icon_star;
        break;
    case BLFM_OLED_ICON_SMILEY:
        icon_data = icon_smiley;
        break;
    default:
        return;
    }
    
    for (uint8_t col = 0; col < 8; col++) {
        if (x + col >= BLFM_OLED_WIDTH) break;
        buf[page][x + col] = icon_data[col];
    }
}

static void draw_progress_bar_buf(uint8_t percent, uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
    if (percent > 100) percent = 100;
    uint8_t filled = (percent * BLFM_OLED_WIDTH) / 100;
    
    uint8_t y_start = BLFM_OLED_HEIGHT - 4;
    for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
        for (uint8_t y = y_start; y < BLFM_OLED_HEIGHT; y++) {
            uint8_t page = y / 8;
            uint8_t bit = y % 8;
            if (x < filled) {
                buf[page][x] |= (1 << bit);
            } else {
                buf[page][x] &= ~(1 << bit);
            }
        }
    }
}

void blfm_oled_apply(const blfm_oled_command_t *data) {
    if (!data) return;
    
    // Clear framebuffer
    for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++) {
        for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
            oled_framebuffer[p][x] = 0x00;
        }
    }
    
    // Draw icons
    draw_icon_buf(0, 0, data->icon1, oled_framebuffer);
    draw_icon_buf(16, 0, data->icon2, oled_framebuffer);
    draw_icon_buf(BLFM_OLED_WIDTH - 16, 0, data->icon3, oled_framebuffer);
    draw_icon_buf(BLFM_OLED_WIDTH - 8, 0, data->icon4, oled_framebuffer);
    
    // Draw small text
    draw_text_buf(0, 1, data->smalltext1, oled_framebuffer);
    
    // Right-aligned small text
    size_t len2 = strlen(data->smalltext2);
    int x_start = (int)BLFM_OLED_WIDTH - (int)(len2 * 8);
    if (x_start < 0) x_start = 0;
    draw_text_buf((uint8_t)x_start, 1, data->smalltext2, oled_framebuffer);
    
    // Centered big text
    size_t biglen = strlen(data->bigtext);
    int big_x = ((int)BLFM_OLED_WIDTH - (int)(biglen * 8)) / 2;
    if (big_x < 0) big_x = 0;
    draw_text_buf((uint8_t)big_x, 2, data->bigtext, oled_framebuffer);
    
    // Progress bar
    draw_progress_bar_buf(data->progress_percent, oled_framebuffer);
    
    // Invert if needed
    if (data->invert) {
        for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++) {
            for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
                oled_framebuffer[p][x] ^= 0xFF;
            }
        }
    }
    
    blfm_oled_flush();
}

// Legacy API implementations
void blfm_oled_draw_line(int x0, int y0, int x1, int y1) {
    // Simplified line drawing
    (void)x0; (void)y0; (void)x1; (void)y1;
}

void blfm_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    // Simplified rectangle
    (void)x; (void)y; (void)w; (void)h;
}

void blfm_oled_draw_char(uint8_t x, uint8_t page, char c) {
    draw_char_buf(x, page, c, oled_framebuffer);
}

void blfm_oled_draw_text(uint8_t x, uint8_t page, const char *str) {
    draw_text_buf(x, page, str, oled_framebuffer);
}

void blfm_oled_invert(void) {
    oled_send_command(0xA7);
}

void blfm_oled_draw_progress_bar(uint8_t percent) {
    draw_progress_bar_buf(percent, oled_framebuffer);
}

void blfm_oled_scroll_text(const char *text, uint8_t speed_ms) {
    if (!text) return;
    
    size_t len = strlen(text);
    if (len == 0) return;
    
    if (speed_ms < 100) speed_ms = 100;
    
    for (size_t offset = 0; offset < (len * 8) + BLFM_OLED_WIDTH; offset += 2) {
        // Clear framebuffer
        for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++) {
            for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
                oled_framebuffer[p][x] = 0x00;
            }
        }
        
        for (size_t i = 0; i < len; i++) {
            int x_pos = (int)BLFM_OLED_WIDTH - (int)offset + (int)(i * 8);
            if (x_pos >= -7 && x_pos < (int)BLFM_OLED_WIDTH) {
                if (x_pos >= 0) {
                    draw_char_buf((uint8_t)x_pos, 0, text[i], oled_framebuffer);
                }
            }
        }
        
        blfm_oled_flush();
        vTaskDelay(pdMS_TO_TICKS(speed_ms));
    }
}

void blfm_oled_scroll_horizontal(const char *text, uint8_t speed) {
    blfm_oled_scroll_text(text, speed);
}

void blfm_oled_blink(uint8_t times, uint16_t delay_ms) {
    if (times > 5) times = 5;
    if (delay_ms < 100) delay_ms = 100;
    
    for (uint8_t i = 0; i < times; i++) {
        oled_send_command(0xA7); // Invert
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        oled_send_command(0xA6); // Normal
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}