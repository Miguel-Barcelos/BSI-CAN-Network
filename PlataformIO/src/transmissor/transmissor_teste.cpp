#include <Arduino.h>
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);
    //twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    // Mude de TWAI_MODE_NORMAL para TWAI_MODE_NO_ACK
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    twai_driver_install(&g_config, &t_config, &f_config);
    twai_start();
    Serial.println(">>> TX PRONTO - 125kbps <<<");
}

void loop()
{
    twai_message_t msg = {.identifier = 0x100, .data_length_code = 1, .data = {0xAA}};

    esp_err_t res = twai_transmit(&msg, pdMS_TO_TICKS(10));

    if (res == ESP_OK)
    {
        Serial.println("OK: Mensagem na rede");
    }
    else
    {
        Serial.printf("ERRO: 0x%X | ", res);

        twai_status_info_t status;
        twai_get_status_info(&status);
        Serial.printf("Estado: %d | Erros TX: %d\n", status.state, status.tx_error_counter);

        if (status.state == TWAI_STATE_BUS_OFF)
        {
            twai_initiate_recovery(); // Tenta voltar à vida sozinho
            Serial.println("Recuperando de Bus-Off...");
        }
    }
    delay(500); // Envia a cada meio segundo para não inundar o log
}