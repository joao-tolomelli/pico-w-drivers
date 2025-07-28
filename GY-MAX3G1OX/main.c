#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define SDA_PIN 2
#define SCL_PIN 3

#define BUTTON_PIN 5
#define MAX30105_ADDRESS 0x57

void i2c_write(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, MAX30105_ADDRESS, buffer, 2, false);
}

uint8_t i2c_read(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(I2C_PORT, MAX30105_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MAX30105_ADDRESS, &value, 1, false);
    return value;
}

void max30105_init() {
    // Reset
    i2c_write(0x09, 0x40); // Mode config: reset
    sleep_ms(100);

    // FIFO config
    i2c_write(0x08, 0x0F); // Sample average = 1, FIFO rollover enabled, almost full = 0x0F
    i2c_write(0x02, 0x00); // FIFO write pointer
    i2c_write(0x03, 0x00); // FIFO overflow
    i2c_write(0x04, 0x00); // FIFO read pointer

    // Mode config
    i2c_write(0x09, 0x03); // SPO2 mode

    // SPO2 config
    i2c_write(0x0A, 0x27); // 100 samples/sec, ADC range 4096nA, 16-bit

    // LED Pulse Amplitudes
    i2c_write(0x0C, 0x24); // LED1 (Red)
    i2c_write(0x0D, 0x24); // LED2 (IR)
}

void read_fifo_sample() {
    uint8_t reg = 0x07; // FIFO_DATA register
    uint8_t data[6];

    i2c_write_blocking(I2C_PORT, MAX30105_ADDRESS, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MAX30105_ADDRESS, data, 6, false);

    uint32_t red = ((uint32_t)(data[0] & 0x03) << 16) | (data[1] << 8) | data[2];
    uint32_t ir  = ((uint32_t)(data[3] & 0x03) << 16) | (data[4] << 8) | data[5];

    printf("Batimentos (IR): %lu | Oxigênio (Red): %lu\n", ir, red);
}

void wait_for_button_press() {
    printf("Aperte o botão para iniciar a leitura...\n");
    while (gpio_get(BUTTON_PIN)) {
        tight_loop_contents();
    }
    sleep_ms(50);
    while (!gpio_get(BUTTON_PIN)) {
        tight_loop_contents();
    }
    sleep_ms(50);
}

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    sleep_ms(500);
    uint8_t id = i2c_read(0xFF);
    printf("Registrador ID: 0x%02X\n", id);

    if (id != 0x15) {
        printf("Sensor não detectado. Abortando.\n");
        return 1;
    }

    printf("Sensor detectado. Aguarde...\n");
    max30105_init();
    printf("Modo SPO2 ativado. Sensor configurado.\n");

    while (1) {
        wait_for_button_press();
        printf("Lendo valores do sensor...\n");
        for (int i = 0; i < 10; i++) {
            read_fifo_sample();
            sleep_ms(500);
        }
        printf("Leitura finalizada.\n\n");
    }

    return 0;
}
