#ifndef VL53L0X_h
#define VL53L0X_h

// Definições de I2C
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define VL53L0X_ADDR 0x29

// Lista de endereços de registradores
#define REG_IDENTIFICATION_MODEL_ID   0xC0
#define REG_SYSRANGE_START            0x00
#define REG_RESULT_RANGE_STATUS       0x14
#define REG_RESULT_RANGE_MM           0x1E

int config_i2c();
int vl53l0x_init(i2c_inst_t *i2c);
int vl53l0x_read_distance_mm(i2c_inst_t *i2c);

#endif // VL53L0X_h