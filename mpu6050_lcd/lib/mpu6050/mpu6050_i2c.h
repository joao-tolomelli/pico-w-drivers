#ifndef MPU6050_I2C_H // include guard
#define MPU6050_I2C_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define MPU6050_ADDR 0x68

#define ACCEL_SENS_2G  16384.0f
#define ACCEL_SENS_4G  8192.0f
#define ACCEL_SENS_8G  4096.0f
#define ACCEL_SENS_16G 2048.0f

void mpu6050_setup_i2c(void);
void mpu6050_reset(void);
uint8_t mpu6050_get_accel_range(void); // Returns 0=±2g, 1=±4g, 2=±8g, 3=±16g
void mpu6050_set_accel_range(uint8_t range) ; // 0=±2g, 1=±4g, 2=±8g, 3=±16g
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
bool mpu6050_test(void);

#endif // MPU6050_I2C_H