
#include "blfm_oled.h"
#include "blfm_i2c1.h"
#include "libc_stubs.h"

#define OLED_I2C_ADDR 0x3C

static uint8_t oled_framebuffer[BLFM_OLED_PAGES][BLFM_OLED_WIDTH];

static void oled_send_command(uint8_t cmd) {
  uint8_t buffer[2];
  buffer[0] = 0x00; // Control byte for command
  buffer[1] = cmd;
  blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2);
}

static void oled_send_data(uint8_t data) {
  uint8_t buffer[2];
  buffer[0] = 0x40; // Control byte for data
  buffer[1] = data;
  blfm_i2c1_write(OLED_I2C_ADDR, buffer, 2);
}

void blfm_oled_init(void) {
  oled_send_command(0xAE); // Display OFF
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
  oled_send_command(0xAF); // Display ON

  blfm_oled_clear();
  blfm_oled_flush();
}

void blfm_oled_clear(void) {
  for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      oled_framebuffer[page][col] = 0x00;
    }
  }
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
    oled_send_command(0xB0 | page); // Set page
    oled_send_command(0x00);        // Set lower column address
    oled_send_command(0x10);        // Set higher column address

    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      oled_send_data(oled_framebuffer[page][col]);
    }
  }
}

void blfm_oled_apply(const blfm_oled_command_t *cmd) {
  if (!cmd)
    return;
  for (uint8_t page = 0; page < BLFM_OLED_PAGES; page++) {
    for (uint8_t col = 0; col < BLFM_OLED_WIDTH; col++) {
      oled_framebuffer[page][col] = cmd->buffer[page][col];
    }
  }
  blfm_oled_flush();
}
