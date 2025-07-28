//
// Este programa lê a distância de um sensor VL53L0X usando I2C
// Esta biblioteca esta escrita em C++ e é compatível com o padrão C++17
//

#include <stdio.h>
#include "pico/stdlib.h"
#include "VL53L0X.h"

// Conexões do sensor
#define I2C_ID        i2c0
#define I2C_SCL_PIN   0
#define I2C_SDA_PIN   1
#define AJUSTE       30 // Ajuste de distância em milímetros

#define BAUD_RATE     400000   // 400kHz

int main()
{
    stdio_init_all();
// Inicia a interface I2C
  i2c_init (I2C_ID, BAUD_RATE);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  VL53L0X sensor;
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous(100);

    while (true) {

        printf("millimeters: %d\n", (sensor.readRangeContinuousMillimeters()) - AJUSTE);
        sleep_ms(1000);
    }
}
