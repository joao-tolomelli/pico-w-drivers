#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"
#include "bh1750.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

int main()
{
    stdio_init_all();

    i2c_init(I2C_PORT, 100 * 1000);  // 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(100); // Aguarda estabilização

    bh1750_init(I2C_PORT);

    while (true) {
        float lux = bh1750_read_lux(I2C_PORT);
        printf("Luminosidade: %.2f lux\n", lux);
        sleep_ms(1000);
    }
}
