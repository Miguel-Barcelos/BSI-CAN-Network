#include <Arduino.h>
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);

    // Configuração dos Pinos e Protocolo
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); // Velocidade da Rede
    
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        twai_start();
        Serial.println("Transmissor Iniciado");
    }
}

void loop()
{
    twai_message_t message;
    message.identifier = 0x100;
    message.data_length_code = 4;
    for (int i = 0; i < 4; i++)
        message.data[i] = i;

    // Tenta transmitir
    esp_err_t res = twai_transmit(&message, pdMS_TO_TICKS(100));

    if (res == ESP_OK)
    {
        Serial.println("Mensagem enviada!");
    }
    else
    {
        Serial.print("Erro no envio: ");

        // Verifica o estado do barramento
        twai_status_info_t status_info;
        twai_get_status_info(&status_info);

        if (status_info.state == TWAI_STATE_BUS_OFF)
        {
            Serial.println("Bus-Off! Reiniciando driver...");
            twai_initiate_recovery(); // Inicia recuperação de erro
        }
        else
        {
            Serial.println("Falha de ACK ou Ruído.");
        }
    }
    delay(1000);
}