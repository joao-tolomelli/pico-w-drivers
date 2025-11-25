/****************************************************
 *            BitDogLab + FreeRTOS + SSD1306
 * Projeto: Exemplo FreeRTOS no Raspberry Pi Pico W
 * 
 * Autor: Eng. José Adriano Filho
 * Data: 25/11/2025
 * Descrição: 
 * Exemplo de FreeRTOS na BitDogLab Raspberry Pi Pico W
 * com display SSD1306 via I2C e conexão WiFi.
 * Mostra status da conexão no display.
 * Utiliza semáforos para sincronização.
 * 
 * Template para servir de guia para utilização do
 * FreeRTOS em projetos com BitDogLab.
 * 
 * Tenham atenção ao CMakeLists.txt para inclusão
 * correta das bibliotecas do FreeRTOS e do caminho 
 * nas pastas da lib: ssd1306 se forem utilizar.
 * 
 * Copiar os arquivo FreeRTOSConfig.h para a pasta raiz
 * do projeto.
 * 
 * A biblioteca do SSD1306 está modificada para
 * trabalhar com o FreeRTOS (uso de vTaskDelay ao invés
 * de sleep_ms), e incorporada as proteções necessárias
 * para uso com FreeRTOS.
 * 
 * Importante: copiar a pata do FreeRTOS para dentro da
 * paste do projeto.
 ****************************************************/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Biblioteca SSD1306 externa
#include "ssd1306.h"

// I2C configuratção (usando sua escolha)
#define I2C_PORT   i2c1
#define SDA_PIN    14
#define SCL_PIN    15

/****************************************************
 *                PROTÓTIPOS DAS TASKS
 ****************************************************/
void wifi_task(void *pv);
void display_task(void *pv);

/****************************************************
 *                VARIÁVEIS GLOBAIS
 ****************************************************/
SemaphoreHandle_t xDisplaySem;   // sinaliza quando atualizar display
SemaphoreHandle_t xStatusMutex;  // protege as mensagens

char wifi_status_msg[64];
char wifi_ip_msg[32];

/****************************************************
 *                     MAIN
 ****************************************************/
int main() 
{
    stdio_init_all();

    // Configura I2C1
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Cria semáforos
    xDisplaySem = xSemaphoreCreateBinary();
    xStatusMutex = xSemaphoreCreateMutex();

    // Inicializa display (biblioteca externa)
    ssd1306_init(I2C_PORT);

    // Mensagens iniciais
    strcpy(wifi_status_msg, "Iniciando...");
    strcpy(wifi_ip_msg, "----");

    xSemaphoreGive(xDisplaySem);

    // Cria tasks
    xTaskCreate(wifi_task, "wifi", 4096, NULL, 2, NULL);
    xTaskCreate(display_task, "display", 3072, NULL, 1, NULL);

    // inicia FreeRTOS
    vTaskStartScheduler();

    while (true) {
        // Nunca deve chegar aqui
    };
}


/****************************************************
 *                TASK PARA O WIFI
 ****************************************************/
#define WIFI_SSID      "Lu e Deza"
#define WIFI_PASSWORD  "liukin1208"

void wifi_task(void *pv) 
{
    if (cyw43_arch_init()) {
        strcpy(wifi_status_msg, "ERRO WiFi INIT");
        xSemaphoreGive(xDisplaySem);
        vTaskDelete(NULL);
    }

    cyw43_arch_enable_sta_mode();

    for (;;) 
    {
        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        strcpy(wifi_status_msg, "Conectando...");
        strcpy(wifi_ip_msg, "----");
        xSemaphoreGive(xStatusMutex);
        xSemaphoreGive(xDisplaySem);

        int r = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            WIFI_PASSWORD,
            CYW43_AUTH_WPA2_AES_PSK,
            15000);

        if (r == 0) {
            // Obtem IP
            uint8_t *ip = (uint8_t*)&cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;

            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            strcpy(wifi_status_msg, "Conectado");
            snprintf(wifi_ip_msg, sizeof(wifi_ip_msg),
                     "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
            xSemaphoreGive(xStatusMutex);

            xSemaphoreGive(xDisplaySem);

            vTaskDelay(pdMS_TO_TICKS(6000));
        }
        else {
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            strcpy(wifi_status_msg, "Falha WiFi");
            strcpy(wifi_ip_msg, "----");
            xSemaphoreGive(xStatusMutex);

            xSemaphoreGive(xDisplaySem);

            vTaskDelay(pdMS_TO_TICKS(4000));
        }
    }
}

/****************************************************
 *                 TASK PARA O DISPLAY
 ****************************************************/
void display_task(void *pv) 
{
    for (;;) 
    {
        xSemaphoreTake(xDisplaySem, portMAX_DELAY);

        char st[64];
        char ip[32];

        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        strcpy(st, wifi_status_msg);
        strcpy(ip, wifi_ip_msg);
        xSemaphoreGive(xStatusMutex);

        ssd1306_clear();
        ssd1306_draw_string(10, 0, "BitDogLab FreeRTOS");
        ssd1306_draw_string(0, 15, "Status:");
        ssd1306_draw_string(50, 15, st);
        ssd1306_draw_string(0, 35, "IP:");
        ssd1306_draw_string(20, 35, ip);
        ssd1306_show();
    }
}
