
#include "ssd1306.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string.h>
#include "font6x8.h"

static uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static i2c_inst_t *ssd_i2c;

static void ssd1306_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(ssd_i2c, SSD1306_I2C_ADDR, buf, 2, false);
}

static void ssd1306_data(uint8_t *data, size_t len) {
    uint8_t buf[len + 1];
    buf[0] = 0x40;
    memcpy(&buf[1], data, len);
    i2c_write_blocking(ssd_i2c, SSD1306_I2C_ADDR, buf, len + 1, false);
}

void ssd1306_init(i2c_inst_t *i2c) {
    ssd_i2c = i2c;

    ssd1306_command(0xAE);
    ssd1306_command(0xA8); ssd1306_command(0x3F);
    ssd1306_command(0xD3); ssd1306_command(0x00);
    ssd1306_command(0x40);
    ssd1306_command(0xA1);
    ssd1306_command(0xC8);
    ssd1306_command(0xDA); ssd1306_command(0x12);
    ssd1306_command(0x81); ssd1306_command(0x7F);
    ssd1306_command(0xA4);
    ssd1306_command(0xA6);
    ssd1306_command(0xD5); ssd1306_command(0x80);
    ssd1306_command(0x8D); ssd1306_command(0x14);
    ssd1306_command(0xAF);

    ssd1306_clear();
    ssd1306_show();
}

void ssd1306_clear(void) {
    memset(buffer, 0, sizeof(buffer));
}

void ssd1306_show(void) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_data(&buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
    }
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, bool color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    if (color)
        buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    else
        buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *text) {
    while (*text) {
        char c = *text++;
        if (c < 32 || c > 126) c = '?';
        for (uint8_t i = 0; i < 5; i++) {
            uint8_t line = font6x8[(c - 32) * 5 + i];
            for (uint8_t j = 0; j < 8; j++) {
                bool pixel = line & (1 << j);
                ssd1306_draw_pixel(x + i, y + j, pixel);
            }
        }
        x += 6;
    }
}
