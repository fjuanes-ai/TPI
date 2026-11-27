#include "wifi_app.h"
#include <WiFi.h>

static const char* AP_SSID = "Robot_ESP32";
static const char* AP_PASS = "robot1234";

void iniciarAccessPoint()
{
    WiFi.softAP(AP_SSID, AP_PASS); /*empieza a transmitir su propia red wifi*/
    WiFi.setSleep(false); /*desactiva modo de ahorro de energia del wifi*/
}