#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "aht10.h"
#include "bh1750.h"


#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

// Prototipos das funções I2C
int i2c_write(uint8_t addr, const uint8_t *data, uint16_t len);
int i2c_read(uint8_t addr, uint8_t *data, uint16_t len);
void delay_ms(uint32_t ms);

int main()
{
    stdio_init_all();

    i2c_init(I2C_PORT, 100 * 1000);  // 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    bh1750_init(I2C_PORT);

    // Define estrutura do sensor
    AHT10_Handle aht10 = {
        .iface = {
            .i2c_write = i2c_write,
            .i2c_read = i2c_read,
            .delay_ms = delay_ms
        }
    };

    // Inicializa o sensor AHT10
    printf("Inicializando AHT10...\n");
    if (!AHT10_Init(&aht10)) {
        printf("Falha na inicialização do sensor!\n");
        while (1) sleep_ms(1000);
    }

    while (true) {
       float lux = bh1750_read_lux(I2C_PORT);
        printf("Luminosidade: %.2f lux\n", lux);
        sleep_ms(1000);

        float temp, hum;
        if (AHT10_ReadTemperatureHumidity(&aht10, &temp, &hum)) {
            printf("Temperatura: %.2f °C | Umidade: %.2f %%\n", temp, hum);
        } else {
            printf("Falha na leitura dos dados!\n");
        }
    }
}

// Função para escrita I2C
int i2c_write(uint8_t addr, const uint8_t *data, uint16_t len) {
    int result = i2c_write_blocking(I2C_PORT, addr, data, len, false);
    return result < 0 ? -1 : 0;
}

// Função para leitura I2C
int i2c_read(uint8_t addr, uint8_t *data, uint16_t len) {
    int result = i2c_read_blocking(I2C_PORT, addr, data, len, false);
    return result < 0 ? -1 : 0;
}

// Função para delay
void delay_ms(uint32_t ms) {
    sleep_ms(ms);
}

