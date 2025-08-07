#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "vl53l0x.h"

int config_i2c() {
    i2c_init(I2C_PORT, 100 * 1000); // Comunicação I2C a 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

// Inicializa o sensor VL53L0X
// Retorna 1 em caso de sucesso, 0 em caso de falha
int vl53l0x_init(i2c_inst_t *i2c) {
    uint8_t reg = REG_IDENTIFICATION_MODEL_ID;
    uint8_t id;

    // Lê ID do sensor
    if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
        return 0;
    if (i2c_read_blocking(i2c, VL53L0X_ADDR, &id, 1, false) != 1)
        return 0;
    if (id != 0xEE) {       // Confirma se é o VL53L0X
        printf("ID inválido: 0x%02X (esperado: 0xEE)\n", id);
        return 0;
    }

    return 1;
}

/*
--- LEITURA DO SENSOR VL53L0X ---
*/
int vl53l0x_read_distance_mm(i2c_inst_t *i2c) {
    // Inicia medição única
    uint8_t cmd[2] = {REG_SYSRANGE_START, 0x01};
    if (i2c_write_blocking(i2c, VL53L0X_ADDR, cmd, 2, false) != 2)
        return -1;

    // Aguarda resultado (~50 ms máx)
    for (int i = 0; i < 100; i++) {
        uint8_t reg = REG_RESULT_RANGE_STATUS;
        uint8_t status;

        if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
            return -1;
        if (i2c_read_blocking(i2c, VL53L0X_ADDR, &status, 1, false) != 1)
            return -1;

        if (status & 0x01) break;  // Medição pronta
        sleep_ms(5);
    }

    // Lê os 2 bytes da distância
    uint8_t reg = REG_RESULT_RANGE_MM;
    uint8_t buffer[2];

    if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
        return -1;
    if (i2c_read_blocking(i2c, VL53L0X_ADDR, buffer, 2, false) != 2)
        return -1;

    return (buffer[0] << 8) | buffer[1];
}