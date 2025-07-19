
#ifndef BLFM_OLED_H
#define BLFM_OLED_H

#include "blfm_types.h"
#include <stdint.h>

void blfm_oled_init(void);
void blfm_oled_clear(void);
void blfm_oled_flush(void);
void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void blfm_oled_draw_line(int x0, int y0, int x1, int y1);
void blfm_oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void blfm_oled_draw_char(uint8_t x, uint8_t page, char c);
void blfm_oled_draw_text(uint8_t x, uint8_t page, const char *str);
void blfm_oled_invert(void);
void blfm_oled_draw_progress_bar(uint8_t percent);
void blfm_oled_scroll_horizontal(const char *text, uint8_t speed);
void blfm_oled_scroll_text(const char *text, uint8_t speed_ms);
void blfm_oled_blink(uint8_t times, uint16_t delay_ms);
void blfm_oled_apply(const blfm_oled_command_t *cmd);

#endif
