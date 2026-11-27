#include "sensores.h"
#include <WiFi.h>

#define LPC_RX_PIN 14
#define LPC_TX_PIN 15
#define TRAMA_TAM 7
static const uint16_t TCP_PORT = 3333;

static HardwareSerial LPCSerial(2);
static WiFiServer tcpServer(TCP_PORT);
static WiFiClient tcpClient;
static char tramaBuffer[TRAMA_TAM];
static uint8_t tramaIndex = 0;

static bool leerLineaUART(int16_t *outAc, uint16_t *outUs, uint8_t *outM)
{
    uint8_t c, acAlto, acBajo, UsAlto, usBajo;

    while (LPCSerial.available() > 0)
    {
        c = LPCSerial.read();

        if (tramaIndex == 0)
        {
            if (c == '$') tramaBuffer[tramaIndex++] = c;
        }else
            {
                tramaBuffer[tramaIndex++] = c;
                if (tramaIndex == TRAMA_TAM)
                {
                    tramaIndex = 0;
                    if (tramaBuffer[6] == '%')
                    {
                        acAlto = tramaBuffer[1];
                        acBajo = tramaBuffer[2];
                        usAlto = tramaBuffer[3];
                        usBajo = tramaBuffer[4];

                        *outAc = (int16_t)(((uint16_t)acAlto << 8) | acBajo);
                        *outUs = ((uint16_t)usAlto << 8) | usBajo;
                        *outM  = tramaBuffer[5];
                        return true;
                    }
                }
            }
    }
    return false;
}

void iniciarSensorUART()
{
    pinMode(LPC_RX_PIN, INPUT_PULLUP);   /* evita ruido cuando no hay nada conectado */
    LPCSerial.begin(115200, SERIAL_8N1, LPC_RX_PIN, LPC_TX_PIN);
    tcpServer.begin();
    tcpServer.setNoDelay(true);
}

void procesarSensoresUART()
{
    char json[80];
    uint8_t motor; uint16_t ultra; int16_t accel;

    if (tcpServer.hasClient())
    {
        if (!tcpClient || !tcpClient.connected())
        {
            tcpClient = tcpServer.accept();
            tcpClient.setNoDelay(true);
            Serial.println("[TCP] Cliente Qt conectado");
        }
    }

    if (leerLineaUART(&accel, &ultra, &motor))
    {
        snprintf(json, sizeof(json), "{\"accel\":%d,\"ultra\":%d,\"motor\":%d}\n", accel, ultra, motor);

        Serial.print("[DEBUG] ");
        Serial.print(json);

        if (tcpClient && tcpClient.connected()) tcpClient.print(json);
    }
}

/*

void debugHexUART() 
{
    while (LPCSerial.available() > 0)
    {
        uint8_t c = LPCSerial.read();
        if (c < 0x10) Serial.print('0');
        Serial.print(c, HEX);
        Serial.print(' ');
    }
}

*/
