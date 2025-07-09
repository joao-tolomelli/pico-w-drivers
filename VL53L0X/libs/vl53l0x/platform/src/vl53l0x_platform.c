#include "vl53l0x_platform.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define I2C_PORT i2c0

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    uint8_t buffer[1 + count];
    buffer[0] = index;
    memcpy(&buffer[1], pdata, count);
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, buffer, count + 1, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, &index, 1, true);
    i2c_read_blocking(I2C_PORT, dev->I2cDevAddr, pdata, count, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV dev, uint8_t index, uint8_t data) {
    uint8_t buffer[2] = {index, data};
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, buffer, 2, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV dev, uint8_t index, uint16_t data) {
    uint8_t buffer[3] = {
        index,
        (uint8_t)(data >> 8),
        (uint8_t)(data & 0xFF)
    };
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, buffer, 3, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV dev, uint8_t index, uint32_t data) {
    uint8_t buffer[5] = {
        index,
        (uint8_t)(data >> 24),
        (uint8_t)(data >> 16),
        (uint8_t)(data >> 8),
        (uint8_t)(data & 0xFF)
    };
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, buffer, 5, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV dev, uint8_t index, uint8_t *data) {
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, &index, 1, true);
    i2c_read_blocking(I2C_PORT, dev->I2cDevAddr, data, 1, false);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV dev, uint8_t index, uint16_t *data) {
    uint8_t buffer[2];
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, &index, 1, true);
    i2c_read_blocking(I2C_PORT, dev->I2cDevAddr, buffer, 2, false);
    *data = (buffer[0] << 8) | buffer[1];
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV dev, uint8_t index, uint32_t *data) {
    uint8_t buffer[4];
    i2c_write_blocking(I2C_PORT, dev->I2cDevAddr, &index, 1, true);
    i2c_read_blocking(I2C_PORT, dev->I2cDevAddr, buffer, 4, false);
    *data = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
            ((uint32_t)buffer[2] << 8) | buffer[3];
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
    uint8_t data;
    VL53L0X_RdByte(dev, index, &data);
    data = (data & AndData) | OrData;
    return VL53L0X_WrByte(dev, index, data);
}

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV dev) {
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV dev) {
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV dev) {
    sleep_ms(1);
    return VL53L0X_ERROR_NONE;
}
