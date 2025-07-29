#ifndef MPU6050__H
#define MPU6050__H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

class MPU6050 {

  public:

    typedef struct {
      float x;
      float y;
      float z;
    } VECT_3D;

    // Endereço I2C padrão
    static const uint8_t I2C_ADDR = 0x68;

    // Identificação do dispositivo
    static const uint8_t ID = 0x68;

    // Construtor
    MPU6050 (i2c_inst_t *i2c, uint8_t addr = MPU6050::I2C_ADDR);

    // Métodos públicos
    void    begin(void);
    uint8_t getId(void);
    void    reset(void);
    void    getAccel(VECT_3D *vect, float scale = 16384.0);
    void    getGyro(VECT_3D *vect, float scale = 65.5);
    float   getTemp();

  private:
    
    // Endereços dos registradores
    static const int SMPLRT_DIV = 0x19;
    static const int CONFIG = 0x1A;
    static const int GYRO_CONFIG = 0x1B;
    static const int ACCEL_CONFIG = 0x1C;
    static const int ACCEL_OUT = 0x3B;
    static const int TEMP_OUT = 0x41;
    static const int GYRO_OUT = 0x43;
    static const int SIG_PATH_RESET = 0x68;
    static const int PWR_MGMT_1 = 0x6B;
    static const int PWR_MGMT_2 = 0x6C;
    static const int WHO_AM_I = 0x75;

    // Instância do I2C
    i2c_inst_t *i2c;

    // Endereço I2C do sensor
    uint8_t addr;

    // Identificação do Sensor
    uint8_t id;

    // Dados brutos
    int16_t raw[14];

    // Rotinas privativas
    void     readRaw(void);
    uint8_t  read8(uint8_t reg);
    void     write8(uint8_t reg, uint8_t val);
};


#endif // MPU6050__H