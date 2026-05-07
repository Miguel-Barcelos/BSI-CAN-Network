#include <Arduino.h>
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        twai_start();
        Serial.println("Receptor Pronto e Aguardando...");
    }
}

void loop()
{
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
    {
        Serial.printf("Recebido! ID: 0x%03X | Dados: %02X %02X %02X %02X\n",
                      message.identifier, message.data[0], message.data[1], message.data[2], message.data[3]);
    }
}