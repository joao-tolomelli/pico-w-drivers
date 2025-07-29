#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "tcs34725.h"

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

    tcs34725_write8(REG_ATIME, 0x00);     // 700ms
    tcs34725_write8(REG_CONTROL, 0x02);   // Ganho 16x

    tcs34725_write8(REG_ENABLE, 0x01);    // PON
    sleep_ms(3);                          // Mínimo 2.4ms

    tcs34725_write8(REG_ENABLE, 0x03);    // PON + AEN
    printf("Sensor ligado (PON + AEN)\n");

    sleep_ms(700); // Tempo de integração
}

bool tcs34725_data_ready()
{
    uint8_t status = tcs34725_read8(REG_STATUS);
    return (status & 0x01); // Bit AVALID
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

void tcs34725_normalize_rgb(uint16_t c, uint16_t r, uint16_t g, uint16_t b,
                            uint8_t *out_r, uint8_t *out_g, uint8_t *out_b)
{
    if (c == 0)
    {
        *out_r = *out_g = *out_b = 0;
        return;
    }

    *out_r = (uint8_t)((r * 255) / c);
    *out_g = (uint8_t)((g * 255) / c);
    *out_b = (uint8_t)((b * 255) / c);

    if (*out_r > 255) *out_r = 255;
    if (*out_g > 255) *out_g = 255;
    if (*out_b > 255) *out_b = 255;
}
