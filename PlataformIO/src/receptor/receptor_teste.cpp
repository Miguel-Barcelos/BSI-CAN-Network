#include <Arduino.h>
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);
    // Configuração idêntica de pinos e velocidade
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    // Para:
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_LOOPBACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS(); // Mesma velocidade do TX!

    

    



    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    twai_driver_install(&g_config, &t_config, &f_config);
    twai_start();
    Serial.println(">>> RX PRONTO - Aguardando... <<<");
}

void loop()
{
    twai_message_t msg;
    // Tenta receber uma mensagem (espera até 500ms)
    if (twai_receive(&msg, pdMS_TO_TICKS(500)) == ESP_OK)
    {
        Serial.printf("RECEBIDO! ID: 0x%X | Dado: %02X\n", msg.identifier, msg.data[0]);
    }
    else
    {
        Serial.print("."); // Mostra pontos para sabermos que o loop do RX não travou
    }
}