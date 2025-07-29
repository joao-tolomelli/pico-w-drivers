#include "MPU6050.h"

// Construtor
MPU6050::MPU6050 (i2c_inst_t *i2c, uint8_t addr) {

  this->i2c = i2c;
  this->addr = addr;
}

// Configura o sensor
void MPU6050::begin() {
  reset();
  this->id = read8(WHO_AM_I);
  write8(CONFIG, 0x00);
  sleep_ms(100);
  write8(ACCEL_CONFIG, 0x00);
  write8(GYRO_CONFIG, 0x08);
  write8(SMPLRT_DIV, 0x00);
  write8(PWR_MGMT_1, 0x01);
  write8(PWR_MGMT_2, 0x00);
  sleep_ms(20);
}

// Retorna o ID do sensor
uint8_t MPU6050::getId() {
  return id;
}

// Reinicia o sensor
void MPU6050::reset(void) {
  uint8_t val = read8(PWR_MGMT_1) | 0x80;
  write8(PWR_MGMT_1, val);
  while (val & 0x80) {
    val = read8(PWR_MGMT_1);
    sleep_ms(1);
  }
  val = read8(SIG_PATH_RESET) | 0x07;
  write8(SIG_PATH_RESET, val);
  while (val & 0x07) {
    val = read8(SIG_PATH_RESET);
    sleep_ms(1);
  }
}

// Fornece aceleração em g
void MPU6050::getAccel(VECT_3D *vect, float scale) {
  readRaw();
  vect->x = (float) raw[0] / scale;
  vect->y = (float) raw[1] / scale;
  vect->z = (float) raw[2] / scale;
}

// Lê giroscópio em graus por segundo
void MPU6050::getGyro(VECT_3D *vect, float scale) {
  readRaw();
  vect->x = (float) raw[4] / scale;
  vect->y = (float) raw[5] / scale;
  vect->z = (float) raw[6] / scale;
}

// Lê temperatura em C
float MPU6050::getTemp() {
  readRaw();
  return (float) raw[3]/340.0 + 36.53;
}

// Lê os dados brutos
void MPU6050::readRaw() {
  uint8_t data[14];

  // Seleciona o primeiro registrador
  uint8_t reg = ACCEL_OUT;
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê os valores
  i2c_read_blocking (i2c, addr, data, 14, false);

  // Converte para int16
  for (int i = 0; i < 7; i++) {
    raw[i] = (data[2*i] << 8) | data[2*i+1];
  }
}

// Lê um valor de 8 bits de um registrador
uint8_t MPU6050::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Seleciona o registrador
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê o valor
  i2c_read_blocking (i2c, addr, val, 1, false);
  return val[0];
} 

// Escreve um valor de 8 bits em um registrador
void MPU6050::write8 (uint8_t reg, uint8_t val)  {  
  uint8_t aux[2];

  aux[0] = reg;
  aux[1] = val;
  i2c_write_blocking (i2c, addr, aux, 2, false);
} 
