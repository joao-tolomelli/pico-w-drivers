#include <stdio.h>
#include "pico/stdlib.h"
#include "string.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT1 i2c1
#define I2C_SDA1 14
#define I2C_SCL1 15

int main()
{
    stdio_init_all();

    // Inicializa I2C e OLED
    // Inicializa I2C OLED
    i2c_init(I2C_PORT1, 400000);
    gpio_set_function(I2C_SDA1, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL1, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA1);
    gpio_pull_up(I2C_SCL1);

    ssd1306_init(I2C_PORT1);
    ssd1306_clear();
    ssd1306_draw_string(32, 0, "Embarcatech");
    ssd1306_draw_string(32, 10, "lib ssd1306");
    ssd1306_show();

    uint16_t cont = 0;

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
