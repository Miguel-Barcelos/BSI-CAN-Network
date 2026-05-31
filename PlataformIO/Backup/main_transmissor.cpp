#include <Arduino.h>
#include "driver/twai.h"

// Definição dos Pinos de Entrada
#define POT_PIN 34
#define BTN_PIN 12

void setup()
{
    Serial.begin(115200);
    pinMode(BTN_PIN, INPUT_PULLUP);

    // Configuração do Driver CAN (TWAI)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
    {
        Serial.println("ERRO: falha ao instalar driver TWAI");
        while (true)
        {
            delay(1000);
        }
    }

    if (twai_start() != ESP_OK)
    {
        Serial.println("ERRO: falha ao iniciar TWAI");
        while (true)
        {
            delay(1000);
        }
    }

    Serial.println("BSI Transmissora Pronta!");
}

void loop()
{
    // --- FILTRO DE MÉDIA MÓVEL PARA O POTENCIÔMETRO ---
    long soma = 0;
    for (int i = 0; i < 20; i++)
    {
        soma += analogRead(POT_PIN);
        delay(1); // Pequeno intervalo para estabilização do ADC
    }
    int potMedia = soma / 20;

    // Mapeamento para 0-180 graus
    uint8_t servoAngle = map(potMedia, 0, 4095, 0, 180);

    // --- LEITURA DO BOTÃO ---
    int btnRaw = digitalRead(BTN_PIN);
    // Se usar INPUT_PULLUP: LOW (0) é apertado, HIGH (1) é solto
    uint8_t btnState = (btnRaw == LOW) ? 1 : 0;

    // --- ENVIO CAN ---
    twai_message_t msgServo = {};
    msgServo.identifier = 0x220;
    msgServo.data_length_code = 1;
    msgServo.data[0] = servoAngle;
    msgServo.flags = TWAI_MSG_FLAG_NONE;
    if (twai_transmit(&msgServo, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        Serial.println("ERRO: falha ao transmitir mensagem do servo");
    }

    twai_message_t msgLed = {};
    msgLed.identifier = 0x120;
    msgLed.data_length_code = 1;
    msgLed.data[0] = btnState;
    msgLed.flags = TWAI_MSG_FLAG_NONE;
    if (twai_transmit(&msgLed, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        Serial.println("ERRO: falha ao transmitir mensagem do LED");
    }

    // Debug mais limpo
    Serial.print("Pot Médio: ");
    Serial.print(potMedia);
    Serial.print(" | Ângulo: ");
    Serial.print(servoAngle);
    Serial.print(" | Botão: ");
    Serial.println(btnState ? "APERTADO" : "SOLTO");

    esp_err_t res = twai_transmit(&msgServo, pdMS_TO_TICKS(100));
    if (res != ESP_OK)
    {
        if (res == ESP_ERR_TIMEOUT)
            Serial.println("Erro: Timeout (Falta de ACK/Resistor)");
        else
            Serial.printf("Erro CAN: 0x%X\n", res);
    }

    delay(100);
}