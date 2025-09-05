#include "mpu6050_i2c.h"



void mpu6050_setup_i2c() {
    i2c_init(I2C_PORT, 400*1000); // common options: 100*1000 (100 kHz) or 400*1000 (400 kHz)
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Reseta o MPU6050
// O reset é feito escrevendo 0x80 no registro 0x6B (PWR_MGMT_1)
// Após o reset, é necessário aguardar 100 ms antes de reconfigurar o dispositivo
void mpu6050_reset() {
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
    sleep_ms(100);
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
    sleep_ms(10);
}

// Returns 0=±2g, 1=±4g, 2=±8g, 3=±16g
uint8_t mpu6050_get_accel_range() {
    uint8_t reg = 0x1C;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &val, 1, false);
    return (val >> 3) & 0x03; // bits 4:3
}

// 0=±2g, 1=±4g, 2=±8g, 3=±16g
void mpu6050_set_accel_range(uint8_t range) {
    uint8_t buf[2];
    buf[0] = 0x1C; // ACCEL_CONFIG register
    buf[1] = range << 3; // bits 3 e 4
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
}

// le os dados brutos do acelerômetro, giroscópio e temperatura
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];
    uint8_t reg = 0x3B; //MPU6050_REG_ACCEL_XOUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 6, false);
    for (int i=0; i<3; i++)
        accel[i] = (buffer[2*i]<<8) | buffer[2*i+1];

    reg = 0x43; //MPU6050_REG_GYRO_XOUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 6, false);
    for (int i=0; i<3; i++)
        gyro[i] = (buffer[2*i]<<8) | buffer[2*i+1];

    reg = 0x41; //MPU6050_REG_TEMP_OUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 2, false);
    *temp = (buffer[0]<<8) | buffer[1];
}

// Função para testar o MPU6050
bool mpu6050_test(void) {
    uint8_t reg = 0x75; // WHO_AM_I
    static uint8_t id = 0;
    // Lê o WHO_AM_I do MPU6050
    // O valor esperado é 0x70
    int ret = i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    if (ret < 0) return false;
    ret = i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &id, 1, false);
    if (ret < 0) return false;
    if (id != 0x70) return false;
    // Verifica se o MPU6050 está respondendo
    // Lê os dados brutos do acelerômetro e giroscópio
    // Testa leitura dos dados brutos
    int16_t accel[3], gyro[3], temp;
    mpu6050_read_raw(accel, gyro, &temp);
    // Verifica se valores não são todos zero (sensor desconectado)
    if (accel[0] == 0 && accel[1] == 0 && accel[2] == 0) return false;
    return true;
}