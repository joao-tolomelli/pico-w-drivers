#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "vl53l0x.h"

int main()
{
    stdio_init_all();

    config_i2c();       // Configuração de I2C para o VL53L0X
                        // Estamos utilizando a I2c0 com SDA no pino 0 e SCL no pino 1
                        // Caso queira modificar, altere as definições em vl53l0x.h
    sleep_ms(100);      // Aguarda estabilização do I2C

    // Inicializa o VL53L0X
    if (!vl53l0x_init(I2C_PORT)) {
        printf("Falha ao inicializar o VL53L0X.\n");
        while (true) {
            sleep_ms(1000); // Loop infinito em caso de erro
        }
    }

    while (true) {
        int distancia = vl53l0x_read_distance_mm(I2C_PORT);
        
        if (distancia < 0) {
            printf("Erro na leitura da distância.\n");
        } else {
            printf("Distância: %d mm (%.2f m)\n", distancia, distancia / 1000.0f);
        }
        sleep_ms(1000);
    }
}
