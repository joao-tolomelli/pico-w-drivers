#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

#define I2C_PORT i2c0
const uint I2C_SDA = 1;
const uint I2C_SCL = 2;

int main()
{
    printf("Iniciando...\n");
    stdio_init_all();
    sleep_ms(200);

    printf("Iniciando I2C...\n");
    // Inicializa o I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Delay para estabilizar
    sleep_ms(100);

    printf("Iniciando VL53L0X...\n");
    // Configura o dispositivo
    VL53L0X_Dev_t dev = {
        .I2cDevAddr = 0x29, // endereço padrão do VL53L0X
        .comms_type = 1,
        .comms_speed_khz = 100};
    VL53L0X_Error status;

    // Inicialização do sensor
    status = VL53L0X_DataInit(&dev);
    if (status)
    {
        printf("Erro em VL53L0X_DataInit: %d\n", status);
        return 1;
    }

    status = VL53L0X_StaticInit(&dev);
    if (status)
    {
        printf("Erro em VL53L0X_StaticInit: %d\n", status);
        return 1;
    }

    status = VL53L0X_PerformRefCalibration(&dev, NULL, NULL);
    if (status)
    {
        printf("Erro em VL53L0X_PerformRefCalibration: %d\n", status);
        return 1;
    }

    status = VL53L0X_SetDeviceMode(&dev, VL53L0X_DEVICEMODE_SINGLE_RANGING);
    if (status)
    {
        printf("Erro em VL53L0X_SetDeviceMode: %d\n", status);
        return 1;
    }

    printf("VL53L0X inicializado com sucesso.\n");

    printf("Iniciando loop de leitura de distância...\n");
    // Loop de leitura de distância
    while (true)
    {
        VL53L0X_RangingMeasurementData_t medida;

        status = VL53L0X_PerformSingleRangingMeasurement(&dev, &medida);
        if (status == VL53L0X_ERROR_NONE)
        {
            printf("Distância medida: %d mm\n", medida.RangeMilliMeter);
        }
        else
        {
            printf("Erro na medição: %d\n", status);
        }

        sleep_ms(500); // intervalo entre leituras
    }

    return 0;
}
