#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define TCS34725_ADDR 0x29
#define TCS34725_COMMAND_BIT 0x80

#define REG_ENABLE 0x00
#define REG_ATIME 0x01
#define REG_CONTROL 0x0F
#define REG_ID 0x12
#define REG_STATUS 0x13

#define REG_CDATAL 0x14
#define REG_RDATAL 0x16
#define REG_GDATAL 0x18
#define REG_BDATAL 0x1A

void tcs34725_write8(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {TCS34725_COMMAND_BIT | reg, value};
    int result = i2c_write_blocking(i2c0, TCS34725_ADDR, data, 2, false);
    if (result < 0)
    {
        printf("Erro ao escrever no registrador 0x%02X\n", reg);
    }
}

uint8_t tcs34725_read8(uint8_t reg)
{
    uint8_t val;
    uint8_t cmd = TCS34725_COMMAND_BIT | reg;
    i2c_write_blocking(i2c0, TCS34725_ADDR, &cmd, 1, true);
    i2c_read_blocking(i2c0, TCS34725_ADDR, &val, 1, false);
    return val;
}

uint16_t tcs34725_read16(uint8_t reg)
{
    uint8_t cmd = TCS34725_COMMAND_BIT | reg;
    uint8_t buffer[2];
    i2c_write_blocking(i2c0, TCS34725_ADDR, &cmd, 1, true);
    i2c_read_blocking(i2c0, TCS34725_ADDR, buffer, 2, false);
    return (uint16_t)(buffer[1] << 8 | buffer[0]);
}

void tcs34725_init()
{
    printf("Inicializando sensor TCS34725...\n");

    uint8_t id = tcs34725_read8(REG_ID);
    printf("ID do sensor: 0x%02X\n", id);
    if (id == 0x44)
    {
        printf("Sensor TCS34725 detectado.\n");
    }
    else if (id == 0x4D)
    {
        printf("Sensor TCS34727 detectado.\n");
    }
    else
    {
        printf("Sensor TCS3472x não reconhecido. ID: 0x%02X\n", id);
        return;
    }

    // Define tempo de integração (0x00 = 700ms)
    tcs34725_write8(REG_ATIME, 0x00);

    // Define ganho de 16x (0x02)
    tcs34725_write8(REG_CONTROL, 0x02);

    // Liga PON primeiro (bit 0)
    tcs34725_write8(REG_ENABLE, 0x01);
    sleep_ms(3); // Espera mínima de 2.4ms (datasheet)

    // Liga AEN (bit 1) mantendo PON (bit 0)
    tcs34725_write8(REG_ENABLE, 0x03);
    printf("Sensor ligado (PON + AEN)\n");

    sleep_ms(700); // Aguarda tempo de integração
}

bool tcs34725_data_ready()
{
    uint8_t status = tcs34725_read8(REG_STATUS);
    return (status & 0x01); // Bit AVALID indica se os dados estão prontos
}

void tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b)
{
    if (!tcs34725_data_ready())
    {
        printf("Dados ainda não prontos. Ignorando leitura.\n");
        *c = *r = *g = *b = 0;
        return;
    }

    *c = tcs34725_read16(REG_CDATAL);
    *r = tcs34725_read16(REG_RDATAL);
    *g = tcs34725_read16(REG_GDATAL);
    *b = tcs34725_read16(REG_BDATAL);
}

#include <stdint.h>

// Normaliza os valores RGB com base na intensidade total (Clear)
void tcs34725_normalize_rgb(uint16_t c, uint16_t r, uint16_t g, uint16_t b, uint8_t *out_r, uint8_t *out_g, uint8_t *out_b)
{
    if (c == 0)
    {
        // Evita divisão por zero
        *out_r = *out_g = *out_b = 0;
        return;
    }

    // Normaliza cada canal para o intervalo 0–255 com base no "Clear"
    // Multiplicamos por 255 para escalar a faixa, depois dividimos por C
    *out_r = (uint8_t)((r * 255) / c);
    *out_g = (uint8_t)((g * 255) / c);
    *out_b = (uint8_t)((b * 255) / c);

    // Protege contra valores fora do intervalo 0–255 (em casos extremos)
    if (*out_r > 255)
        *out_r = 255;
    if (*out_g > 255)
        *out_g = 255;
    if (*out_b > 255)
        *out_b = 255;
}

int main()
{
    stdio_init_all();
    sleep_ms(3000); // Aguarda para USB serial conectar

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
