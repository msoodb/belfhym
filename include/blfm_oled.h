
#ifndef BLFM_OLED_H
#define BLFM_OLED_H

#include "blfm_types.h"
#include <stdint.h>

void blfm_oled_init(void);
void blfm_oled_clear(void);
void blfm_oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void blfm_oled_flush(void);
void blfm_oled_apply(const blfm_oled_command_t *cmd);
  
#endif /* BLFM_OLED_H */
