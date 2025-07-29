#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "tcs34725.h"

int main()
{
    stdio_init_all();
    sleep_ms(3000); // Aguarda conexão USB

    printf("Inicializando I2C...\n");
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C); // SDA
    gpio_set_function(1, GPIO_FUNC_I2C); // SCL
    gpio_pull_up(0);
    gpio_pull_up(1);

    tcs34725_init();

    while (true)
    {
        uint16_t c, r, g, b;
        uint8_t r8, g8, b8;

        tcs34725_read_rgbc(&c, &r, &g, &b);
        tcs34725_normalize_rgb(c, r, g, b, &r8, &g8, &b8);

        printf("Raw 16-bit  -> C: %u | R: %u | G: %u | B: %u\n", c, r, g, b);
        printf("RGB 8-bit   -> R: %u | G: %u | B: %u | Hex: #%02X%02X%02X\n\n", r8, g8, b8, r8, g8, b8);
        sleep_ms(1000);
    }
}
