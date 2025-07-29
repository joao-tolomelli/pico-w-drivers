#ifndef TCS34725_H
#define TCS34725_H

#include <stdint.h>
#include <stdbool.h>

#define TCS34725_ADDR 0x29
#define TCS34725_COMMAND_BIT 0x80

// Registradores
#define REG_ENABLE  0x00
#define REG_ATIME   0x01
#define REG_CONTROL 0x0F
#define REG_ID      0x12
#define REG_STATUS  0x13
#define REG_CDATAL  0x14
#define REG_RDATAL  0x16
#define REG_GDATAL  0x18
#define REG_BDATAL  0x1A

// Funções públicas
void tcs34725_init();
bool tcs34725_data_ready();
void tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b);
void tcs34725_normalize_rgb(uint16_t c, uint16_t r, uint16_t g, uint16_t b,
                            uint8_t *out_r, uint8_t *out_g, uint8_t *out_b);

#endif
