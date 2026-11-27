#pragma once
#include <Arduino.h>

void iniciarSensorUART();       /*  configura LPCSerial y el TcpServer  */
void procesarSensoresUART();    /*  llamar en loop()  */
