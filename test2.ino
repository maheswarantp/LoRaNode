#include "RAK811.h"
#include <SoftwareSerial.h>

#define DebugSerial Serial
#define TXPIN 11
#define RXPIN 10

SoftwareSerial ATSerial(RXPIN, TXPIN);

RAK811 RAKLoRa(ATSerial, DebugSerial);

void setup()
{
    DebugSerial.begin(115200);
}

void loop()
{
    DebugSerial.println(RAKLoRa.rk_checkDeviceStatus());
}