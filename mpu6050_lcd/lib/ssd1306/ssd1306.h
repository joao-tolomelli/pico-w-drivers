
#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"

#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64

void ssd1306_init(i2c_inst_t *i2c);
void ssd1306_clear(void);
void ssd1306_show(void);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *text);
void ssd1306_draw_pixel(uint8_t x, uint8_t y, bool color);

#endif
