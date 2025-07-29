#include "aht10.h"

static bool aht10_write_command(AHT10_Handle *dev, uint8_t cmd, uint8_t arg1, uint8_t arg2) {
    uint8_t buf[3] = { cmd, arg1, arg2 };
    return dev->iface.i2c_write(AHT10_I2C_ADDRESS, buf, 3) == 0;
}

bool AHT10_Init(AHT10_Handle *dev) {
    if (!dev) return false;
    if (!AHT10_SoftReset(dev)) return false;
    dev->iface.delay_ms(20);
    dev->initialized = aht10_write_command(dev, AHT10_CMD_INITIALIZE, 0x08, 0x00);
    dev->iface.delay_ms(10);
    return dev->initialized;
}

bool AHT10_SoftReset(AHT10_Handle *dev) {
    if (!dev) return false;
    uint8_t cmd = AHT10_CMD_RESET;
    bool ok = dev->iface.i2c_write(AHT10_I2C_ADDRESS, &cmd, 1) == 0;
    dev->iface.delay_ms(20);
    return ok;
}

bool AHT10_IsBusy(AHT10_Handle *dev) {
    uint8_t status = 0;
    if (dev->iface.i2c_read(AHT10_I2C_ADDRESS, &status, 1) != 0) return true;
    return (status & 0x80) != 0;
}

bool AHT10_ReadTemperatureHumidity(AHT10_Handle *dev, float *temperature, float *humidity) {
    if (!dev || !dev->initialized) return false;

    if (!aht10_write_command(dev, AHT10_CMD_MEASURE, 0x33, 0x00)) return false;
    dev->iface.delay_ms(80);

    uint8_t raw[6];
    if (dev->iface.i2c_read(AHT10_I2C_ADDRESS, raw, 6) != 0) return false;

    if ((raw[0] & 0x80) != 0) return false; // ainda ocupado

    uint32_t raw_hum = ((uint32_t)(raw[1]) << 12) | ((uint32_t)(raw[2]) << 4) | (raw[3] >> 4);
    uint32_t raw_temp = (((uint32_t)(raw[3] & 0x0F)) << 16) | ((uint32_t)(raw[4]) << 8) | raw[5];

    *humidity = (raw_hum * 100.0f) / 1048576.0f;
    *temperature = ((raw_temp * 200.0f) / 1048576.0f) - 50.0f;

    return true;
}
