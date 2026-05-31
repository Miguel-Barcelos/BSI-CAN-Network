#include <Arduino.h>
#include "driver/twai.h"

#define CAN_TX GPIO_NUM_5
#define CAN_RX GPIO_NUM_4

void setup()
{
    Serial.begin(115200);

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(); // Aceita QUALQUER ID para teste

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        twai_start();
        Serial.println(">>> Monitor de Frame CAN Iniciado <<<");
    }
}

void loop()
{
    twai_message_t frame;
    // Espera por uma mensagem por até 1 segundo
    esp_err_t status = twai_receive(&frame, pdMS_TO_TICKS(1000));

    if (status == ESP_OK)
    {
        // MOSTRANDO O FRAME RECEBIDO
        Serial.print("FRAME RECEBIDO -> ");
        Serial.print("ID: 0x");
        Serial.print(frame.identifier, HEX);
        Serial.print(" | DLC: ");
        Serial.print(frame.data_length_code);
        Serial.print(" | Dados: ");

        for (int i = 0; i < frame.data_length_code; i++)
        {
            Serial.printf("%02X ", frame.data[i]);
        }
        Serial.println();
    }
    else
    {
        // Se cair aqui, o ESP32 não detectou nenhum sinal nos pinos 4 e 5
        Serial.println("Erro: Barramento ocioso ou falha física...");

        // Verifica se o driver entrou em estado de erro (Bus-Off)
        twai_status_info_t info;
        twai_get_status_info(&info);
        if (info.state == TWAI_STATE_BUS_OFF)
        {
            Serial.println("ALERTA: Driver em BUS-OFF. Reiniciando...");
            twai_initiate_recovery();
        }
    }
}