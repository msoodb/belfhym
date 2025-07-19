#include "blfm_oled.h"
#include "blfm_delay.h"
#include "blfm_font8x8.h"
#include "blfm_i2c1.h"
#include "blfm_types.h"
#include "libc_stubs.h"
#include "FreeRTOS.h"
#include "task.h"

#define OLED_I2C_ADDR 0x3C
#define OLED_UPDATE_RATE_MS 100  // 10 FPS max for low priority
#define OLED_MAX_RETRIES 3

// State management
static uint8_t oled_framebuffer[BLFM_OLED_PAGES][BLFM_OLED_WIDTH];
static TickType_t last_update_time = 0;
static bool oled_initialized = false;
static bool oled_enabled = true;

// Safe I2C communication with error handling
static bool oled_send_cmd_safe(uint8_t cmd) {
  uint8_t buffer[2] = {0x00, cmd};
  for (uint8_t retry = 0; retry < OLED_MAX_RETRIES; retry++) {
    if (blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2) == 0) {
      return true;
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // Brief delay before retry
  }
  oled_enabled = false; // Disable OLED on persistent failures
  return false;
}

static bool oled_send_data_bulk(const uint8_t *data, size_t len) {
  for (uint8_t retry = 0; retry < OLED_MAX_RETRIES; retry++) {
    if (blfm_i2c1_write(OLED_I2C_ADDR, data, len) == 0) {
      return true;
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  oled_enabled = false;
  return false;
}

// Minimal initialization with error checking
void blfm_oled_init(void) {
  static const uint8_t init_cmds[] = {
    0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40,
    0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x02,
    0x81, 0x8F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
  };
  
  oled_enabled = true;
  
  for (size_t i = 0; i < sizeof(init_cmds); i++) {
    if (!oled_send_cmd_safe(init_cmds[i])) {
      return; // Abort initialization on failure
    }
  }
  
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

// Rate-limited, crash-safe flush
void blfm_oled_flush(void) {
  if (!oled_enabled || !oled_initialized) return;
  
  TickType_t current_time = xTaskGetTickCount();
  if ((current_time - last_update_time) < pdMS_TO_TICKS(OLED_UPDATE_RATE_MS)) {
    return; // Skip update - rate limiting
  }
  
  static uint8_t page_buffer[BLFM_OLED_WIDTH + 1];
  page_buffer[0] = 0x40; // Data mode prefix
  
  for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
    // Set page address - abort on any failure
    if (!oled_send_cmd_safe(0xB0 | page) ||
        !oled_send_cmd_safe(0x00) ||
        !oled_send_cmd_safe(0x10)) {
      return;
    }
    
    // Copy page data
    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      page_buffer[col + 1] = oled_framebuffer[page][col];
    }
    
    // Send page data - abort on failure
    if (!oled_send_data_bulk(page_buffer, BLFM_OLED_WIDTH + 1)) {
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

// Simplified character rendering
static void draw_char_fast(uint8_t x, uint8_t page, char c) {
  if (page >= BLFM_OLED_PAGES || x >= BLFM_OLED_WIDTH) return;
  if ((uint8_t)c < 32 || (uint8_t)c > 127) c = '?';
  
  const uint8_t *glyph = blfm_font8x8_basic[(uint8_t)c];
  
  // Transpose and write directly to framebuffer
  for (uint8_t col = 0; col < 8 && (x + col) < BLFM_OLED_WIDTH; col++) {
    uint8_t column_data = 0;
    for (uint8_t row = 0; row < 8; row++) {
      if (glyph[row] & (1 << col)) {
        column_data |= (1 << row);
      }
    }
    oled_framebuffer[page][x + col] = column_data;
  }
}

static void draw_text_fast(uint8_t x, uint8_t page, const char *text) {
  while (*text && x < BLFM_OLED_WIDTH) {
    draw_char_fast(x, page, *text++);
    x += 8;
  }
}

// Optimized icon rendering
static void draw_icon_fast(uint8_t x, uint8_t page, blfm_oled_icon_t icon) {
  if (icon == BLFM_OLED_ICON_NONE || page >= BLFM_OLED_PAGES) return;
  
  static const uint8_t icons[][8] = {
    {0}, // NONE
    {0x0C, 0x1E, 0x3F, 0x7F, 0x7F, 0x3F, 0x1E, 0x0C}, // HEART
    {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C}, // SMILEY
    {0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00}  // STAR
  };
  
  if (icon > BLFM_OLED_ICON_STAR) return;
  
  for (uint8_t col = 0; col < 8 && (x + col) < BLFM_OLED_WIDTH; col++) {
    oled_framebuffer[page][x + col] = icons[icon][col];
  }
}

// Progress bar with minimal CPU usage
static void draw_progress_bar_fast(uint8_t percent) {
  if (percent > 100) percent = 100;
  uint8_t filled = (percent * BLFM_OLED_WIDTH) / 100;
  
  uint8_t page = BLFM_OLED_PAGES - 1; // Bottom page
  for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
    oled_framebuffer[page][x] = (x < filled) ? 0x0F : 0x01; // 4 pixel high bar
  }
}

// Main layout function - optimized
void blfm_oled_apply(const blfm_oled_command_t *data) {
  if (!data || !oled_enabled) return;
  
  blfm_oled_clear();
  
  // Draw icons efficiently
  draw_icon_fast(0, 0, data->icon1);
  draw_icon_fast(16, 0, data->icon2);
  draw_icon_fast(BLFM_OLED_WIDTH - 16, 0, data->icon3);
  draw_icon_fast(BLFM_OLED_WIDTH - 8, 0, data->icon4);
  
  // Draw text
  draw_text_fast(0, 1, data->smalltext1);
  
  // Right-aligned small text
  size_t len2 = strlen(data->smalltext2);
  int x_start = (int)BLFM_OLED_WIDTH - (int)(len2 * 8);
  if (x_start < 0) x_start = 0;
  draw_text_fast((uint8_t)x_start, 1, data->smalltext2);
  
  // Centered big text
  size_t biglen = strlen(data->bigtext);
  int big_x = ((int)BLFM_OLED_WIDTH - (int)(biglen * 8)) / 2;
  if (big_x < 0) big_x = 0;
  draw_text_fast((uint8_t)big_x, 2, data->bigtext);
  
  // Progress bar
  draw_progress_bar_fast(data->progress_percent);
  
  // Hardware invert if needed
  if (data->invert) {
    for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++) {
      for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
        oled_framebuffer[p][x] ^= 0xFF;
      }
    }
  }
  
  blfm_oled_flush();
}

// Simplified scroll - much less resource intensive
void blfm_oled_scroll_text(const char *text, uint8_t speed_ms) {
  if (!text || !oled_enabled) return;
  
  size_t len = strlen(text);
  if (len == 0) return;
  
  if (speed_ms < 100) speed_ms = 100; // Minimum for performance
  uint8_t step = 4; // Fixed step size for efficiency
  
  for (size_t offset = 0; offset < (len * 8) + BLFM_OLED_WIDTH; offset += step) {
    blfm_oled_clear();
    
    // Simple scrolling without complex calculations
    for (size_t i = 0; i < len; i++) {
      int x_pos = (int)BLFM_OLED_WIDTH - (int)offset + (int)(i * 8);
      if (x_pos >= -7 && x_pos < (int)BLFM_OLED_WIDTH) {
        draw_char_fast((uint8_t)(x_pos < 0 ? 0 : x_pos), 0, text[i]);
      }
    }
    
    blfm_oled_flush();
    vTaskDelay(pdMS_TO_TICKS(speed_ms));
  }
}

// Hardware-based blink
void blfm_oled_blink(uint8_t times, uint16_t delay_ms) {
  if (!oled_enabled || times > 5) return; // Limit for performance
  if (delay_ms < 100) delay_ms = 100;
  
  for (uint8_t i = 0; i < times; i++) {
    oled_send_cmd_safe(0xA7); // Invert
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
    oled_send_cmd_safe(0xA6); // Normal
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
  }
}

// Legacy API compatibility stubs
void blfm_oled_draw_line(int x0, int y0, int x1, int y1) {
  // Simplified line drawing for minimal resource usage
  (void)x0; (void)y0; (void)x1; (void)y1;
}

void blfm_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  // Simplified rectangle for minimal resource usage
  (void)x; (void)y; (void)w; (void)h;
}

void blfm_oled_draw_char(uint8_t x, uint8_t page, char c) {
  draw_char_fast(x, page, c);
}

void blfm_oled_draw_text(uint8_t x, uint8_t page, const char *str) {
  draw_text_fast(x, page, str);
}

void blfm_oled_invert(void) {
  if (oled_enabled) {
    oled_send_cmd_safe(0xA7);
  }
}

void blfm_oled_draw_progress_bar(uint8_t percent) {
  draw_progress_bar_fast(percent);
}

void blfm_oled_scroll_horizontal(const char *text, uint8_t speed) {
  blfm_oled_scroll_text(text, speed);
}