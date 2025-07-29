#include "bh1750.h"
#include "pico/stdlib.h"

void bh1750_init(i2c_inst_t *i2c) {
    uint8_t cmd = 0x10;  // Modo Continuo de Alta Resolução
    i2c_write_blocking(i2c, BH1750_ADDR, &cmd, 1, false);
    sleep_ms(180);  // Tempo de conversão típico
}

float bh1750_read_lux(i2c_inst_t *i2c) {
    uint8_t data[2];
    if (i2c_read_blocking(i2c, BH1750_ADDR, data, 2, false) != 2) {
        return -1.0f;
    }

    uint16_t raw = (data[0] << 8) | data[1];
    return raw / 1.2f;  // Conversão para lux
}
