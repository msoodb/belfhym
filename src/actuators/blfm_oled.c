#include "blfm_oled.h"
#include "blfm_delay.h"
#include "blfm_font8x8.h"
#include "blfm_i2c1.h"
#include "blfm_types.h"
#include "libc_stubs.h"

#define OLED_I2C_ADDR 0x3C

static uint8_t oled_framebuffer[BLFM_OLED_PAGES][BLFM_OLED_WIDTH];

static void oled_send_command(uint8_t cmd) {
  uint8_t buffer[2] = {0x00, cmd};
  blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2);
}

static void oled_send_data(uint8_t data) {
  uint8_t buffer[2] = {0x40, data};
  blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2);
}

void blfm_oled_init(void) {
  oled_send_command(0xAE);
  oled_send_command(0xD5);
  oled_send_command(0x80);
  oled_send_command(0xA8);
  oled_send_command(0x1F);
  oled_send_command(0xD3);
  oled_send_command(0x00);
  oled_send_command(0x40);
  oled_send_command(0x8D);
  oled_send_command(0x14);
  oled_send_command(0x20);
  oled_send_command(0x00);
  oled_send_command(0xA1);
  oled_send_command(0xC8);
  oled_send_command(0xDA);
  oled_send_command(0x02);
  oled_send_command(0x81);
  oled_send_command(0x8F);
  oled_send_command(0xD9);
  oled_send_command(0xF1);
  oled_send_command(0xDB);
  oled_send_command(0x40);
  oled_send_command(0xA4);
  oled_send_command(0xA6);
  oled_send_command(0xAF);

  blfm_oled_clear();
  blfm_oled_flush();
}

void blfm_oled_clear(void) {
  for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++)
    for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++)
      oled_framebuffer[p][x] = 0x00;
}

void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
  if (x >= BLFM_OLED_WIDTH || y >= BLFM_OLED_HEIGHT)
    return;
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  if (color)
    oled_framebuffer[page][x] |= (1 << bit);
  else
    oled_framebuffer[page][x] &= ~(1 << bit);
}

void blfm_oled_flush(void) {
  for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
    oled_send_command(0xB0 | page);
    oled_send_command(0x00);
    oled_send_command(0x10);
    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      oled_send_data(oled_framebuffer[page][col]);
    }
  }
}

// --- Helpers for framebuffer operations ---
static void
blfm_oled_draw_pixel_buf(uint8_t x, uint8_t y, uint8_t color,
                         uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  if (x >= BLFM_OLED_WIDTH || y >= BLFM_OLED_HEIGHT)
    return;
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  if (color)
    buf[page][x] |= (1 << bit);
  else
    buf[page][x] &= ~(1 << bit);
}

static void blfm_oled_clear_buf(uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++)
    for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++)
      buf[p][x] = 0x00;
}

static void blfm_oled_flush_buf(uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
    oled_send_command(0xB0 | page);
    oled_send_command(0x00);
    oled_send_command(0x10);
    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      oled_send_data(buf[page][col]);
    }
  }
}

static void
blfm_oled_invert_buf(uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++)
    for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++)
      buf[p][x] ^= 0xFF;
}

// --- Font alignment helpers ---
// --- Font helpers ---
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

// --- Drawing characters and text ---
void blfm_oled_draw_char_buf(uint8_t x, uint8_t page, char c,
                             uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  if ((uint8_t)c < 32 || (uint8_t)c > 127)
    c = '?';

  const uint8_t *glyph = blfm_font8x8_basic[(uint8_t)c];

  uint8_t transposed[8];
  transpose8x8(glyph, transposed);

  for (uint8_t col = 0; col < 8; col++) {
    if (x + col >= BLFM_OLED_WIDTH)
      break;
    buf[page][x + col] = transposed[col];
  }
}

static void
blfm_oled_draw_text_buf(uint8_t x, uint8_t page, const char *text,
                        uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  while (*text && x < BLFM_OLED_WIDTH) {
    blfm_oled_draw_char_buf(x, page, *text++, buf);
    x += 8;
  }
}

// --- Drawing progress bar ---

static void
blfm_oled_draw_progress_bar_buf(uint8_t percent,
                                uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  if (percent > 100)
    percent = 100;
  uint8_t filled = (percent * BLFM_OLED_WIDTH) / 100;

  uint8_t y_start = BLFM_OLED_HEIGHT - 4;
  for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++) {
    for (uint8_t y = y_start; y < BLFM_OLED_HEIGHT; y++) {
      blfm_oled_draw_pixel_buf(x, y, x < filled, buf);
    }
  }
}

// --- Drawing icons ---

static void
blfm_oled_draw_icon_buf(uint8_t x, uint8_t page, blfm_oled_icon_t icon,
                        uint8_t buf[BLFM_OLED_PAGES][BLFM_OLED_WIDTH]) {
  if (icon == BLFM_OLED_ICON_NONE)
    return;

  static const uint8_t icon_heart[8] = {0b00001100, 0b00011110, 0b00111111,
                                        0b01111111, 0b01111111, 0b00111111,
                                        0b00011110, 0b00001100};

  static const uint8_t icon_star[8] = {0b00010000, 0b00111000, 0b01111100,
                                       0b11111110, 0b01111100, 0b00111000,
                                       0b00010000, 0b00000000};

  static const uint8_t icon_smiley[8] = {0b00111100, 0b01000010, 0b10100101,
                                         0b10000001, 0b10100101, 0b10011001,
                                         0b01000010, 0b00111100};

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
    if (x + col >= BLFM_OLED_WIDTH)
      break;
    buf[page][x + col] = icon_data[col];
  }
}

// --- Apply full layout from command struct ---

void blfm_oled_apply(const blfm_oled_command_t *data) {
  blfm_oled_clear_buf(oled_framebuffer);

  blfm_oled_draw_icon_buf(0, 0, data->icon1, oled_framebuffer);
  blfm_oled_draw_icon_buf(16, 0, data->icon2, oled_framebuffer);
  blfm_oled_draw_icon_buf(BLFM_OLED_WIDTH - 16, 0, data->icon3,
                          oled_framebuffer);
  blfm_oled_draw_icon_buf(BLFM_OLED_WIDTH - 8, 0, data->icon4,
                          oled_framebuffer);

  blfm_oled_draw_text_buf(0, 1, data->smalltext1, oled_framebuffer);

  size_t len2 = strlen(data->smalltext2);
  int x_start = (int)BLFM_OLED_WIDTH - (int)(len2 * 8);
  if (x_start < 0)
    x_start = 0;
  blfm_oled_draw_text_buf((uint8_t)x_start, 1, data->smalltext2,
                          oled_framebuffer);

  size_t biglen = strlen(data->bigtext);
  int big_x = ((int)BLFM_OLED_WIDTH - (int)(biglen * 8)) / 2;
  if (big_x < 0)
    big_x = 0;
  blfm_oled_draw_text_buf((uint8_t)big_x, 2, data->bigtext, oled_framebuffer);

  blfm_oled_draw_progress_bar_buf(data->progress_percent, oled_framebuffer);

  if (data->invert) {
    blfm_oled_invert_buf(oled_framebuffer);
  }

  blfm_oled_flush_buf(oled_framebuffer);
}

// --- Scrolling text animation ---

void blfm_oled_scroll_text(const char *text, uint8_t speed_ms) {
  size_t len = strlen(text);
  size_t scroll_width = len * 8;

  for (size_t offset = 0; offset < scroll_width + BLFM_OLED_WIDTH; offset++) {
    blfm_oled_clear_buf(oled_framebuffer);

    for (size_t i = 0; i < len; i++) {
      int x_pos = (int)BLFM_OLED_WIDTH - (int)offset + (int)(i * 8);
      if (x_pos < -7 || x_pos >= (int)BLFM_OLED_WIDTH)
        continue;

      const uint8_t *glyph = blfm_font8x8_basic[(uint8_t)text[i]];
      uint8_t transposed[8];
      transpose8x8(glyph, transposed);
      for (uint8_t col = 0; col < 8; col++) {
        if ((x_pos + col) >= 0 && (x_pos + col) < (int)BLFM_OLED_WIDTH) {
          oled_framebuffer[0][x_pos + col] = transposed[col];
        }
      }
    }
    blfm_oled_flush_buf(oled_framebuffer);
    blfm_delay_ms(speed_ms);
  }
}

// --- Blink entire display ---

void blfm_oled_blink(uint8_t times, uint16_t delay_ms) {
  for (uint8_t i = 0; i < times; i++) {
    for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++)
      for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++)
        oled_framebuffer[p][x] ^= 0xFF;

    blfm_oled_flush_buf(oled_framebuffer);
    blfm_delay_ms(delay_ms);

    for (uint8_t p = 0; p < BLFM_OLED_PAGES; p++)
      for (uint8_t x = 0; x < BLFM_OLED_WIDTH; x++)
        oled_framebuffer[p][x] ^= 0xFF;

    blfm_oled_flush_buf(oled_framebuffer);
    blfm_delay_ms(delay_ms);
  }
}
