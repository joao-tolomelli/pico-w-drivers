#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MPU6050.h"

// Conexões do sensor'
#define I2C_ID        i2c0
#define I2C_SCL_PIN   0
#define I2C_SDA_PIN   1

#define BAUD_RATE     400000   // 400kHz

int main()
{
    stdio_init_all();

  // Inicia a interface I2C
  uint baud = i2c_init (I2C_ID, BAUD_RATE);
  printf ("I2C @ %u Hz\n", baud);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  // Inicia o sensor
  MPU6050 sensor (I2C_ID);
  sensor.begin();

  // Mostra o ID do sensor
  printf ("ID do Sensor: %02X\n", sensor.getId());

  // Laço Principal
  MPU6050::VECT_3D data;
  while(1) {
    sleep_ms(2000);
    sensor.getAccel(&data);
    printf ("Aceleracao X:%.1f Y:%.1f Z:%.1f\n", data.x, data.y, data.z);
    sensor.getGyro(&data);
    printf ("Giroscopio X:%.1f Y:%.1f Z:%.1f\n", data.x, data.y, data.z);
    printf ("Temperatura: %.1f\n", sensor.getTemp());
    printf("\n");
  }
}