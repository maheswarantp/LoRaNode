/********************************************************
 * This demo is only supported after RUI firmware version 3.0.0.13.X on RAK811
 * Master Board Uart Receive buffer size at least 128 bytes. 
 ********************************************************/
#include <ArduinoJson.h>
StaticJsonDocument<200> doc;
//DynamicJsonDocument doc(1024);

#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

struct JSON{
  float temperature;
  float humidity;
};

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "8680000000000001";
String AppEui = "70B3D57ED00285A7";
String AppKey = "DDDFB1023885FBFF74D3A55202EDF2B1";
#else JOIN_MODE == ABP
String NwkSKey = "438991FE0B6B0856799AF47ABF82E0F1";

String AppSKey = "A10D2742058D5F1D6E539AF79A60FDDE";
String DevAddr = "260410C5";
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial ATSerial(RXpin,TXpin);    // Declare a virtual serial port

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial,DebugSerial);

struct JSON j1;

void setup() {
  dht.begin();
  DebugSerial.begin(115200);
  while(DebugSerial.available())
  {
    DebugSerial.read(); 
  }
  ATSerial.begin(9600); //set ATSerial baudrate:This baud rate has to be consistent with  the baud rate of the WisNode device.
  while(ATSerial.available())
  {
    ATSerial.read(); 
  }

  if(!RAKLoRa.rk_setWorkingMode(0))  //set WisNode work_mode to LoRaWAN.
  {
    DebugSerial.println(F("set work_mode failed, please reset module."));
    while(1);
  }
  
  RAKLoRa.rk_getVersion();  //get RAK811 firmware version
  DebugSerial.println(RAKLoRa.rk_recvData());  //print version number

  DebugSerial.println(F("Start init RAK811 parameters..."));

  if (!InitLoRaWAN())  //init LoRaWAN
  {
    DebugSerial.println(F("Init error,please reset module.")); 
    while(1);
  }

  DebugSerial.println(F("Start to join LoRaWAN..."));
  while(!RAKLoRa.rk_joinLoRaNetwork(60))  //Joining LoRaNetwork timeout 60s
  {
    DebugSerial.println();
    DebugSerial.println(F("Rejoin again after 5s..."));
    delay(5000);
  }
  DebugSerial.println(F("Join LoRaWAN success"));

  if(!RAKLoRa.rk_isConfirm(0))  //set LoRa data send package type:0->unconfirm, 1->confirm
  {
    DebugSerial.println(F("LoRa data send package set error,please reset module.")); 
    while(1);    
  }
}

bool InitLoRaWAN(void)
{
  if(RAKLoRa.rk_setJoinMode(JOIN_MODE))  //set join_mode:ABP
  {
    if(RAKLoRa.rk_setRegion(7))  //set region EU868
    {
      if (RAKLoRa.rk_initABP(DevAddr, NwkSKey, AppSKey))  //set ABP mode parameters
      {
        DebugSerial.println(F("RAK811 init OK!"));  
        return true;    
      }
    }
  }
  return false;
}

String struct_to_string(struct JSON j1){
  String quotes = "\"";
  String jsonStart = "{\"";
  String jsonEnd = "\"}";
  String comma = ",";
  jsonStart += "temperature";
  jsonStart += quotes;
  jsonStart += ":";
  jsonStart += quotes;
  jsonStart += j1.temperature;
  jsonStart += jsonEnd;
  return jsonStart;
}

String text_to_hex(String json)
{
  String hex;
  for(int i=0;i<json.length(); i++){
    hex.concat(String((int)json[i], HEX));
  }
//  Serial.print("The CONVERTED STRING IS ");
  return hex;
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
//  DebugSerial.println("===================");
//  j1.temperature = t;
//  j1.humidity = h;
//  String s1 = struct_to_string(j1);
  doc["temperature"] = t;
  doc["humidity"] = h;
  String json_string;
  serializeJson(doc, json_string);
  Serial.println(json_string);
  DebugSerial.println("===================");
  String hexVal = text_to_hex(json_string);
  int len = (int)hexVal.length() * 2 + 1;
  char buffer[len];
  hexVal.toCharArray(buffer,len);
  DebugSerial.println(F("Start send data..."));
  
  if (RAKLoRa.rk_sendData(1, buffer))
  {    
    for (unsigned long start = millis(); millis() - start < 90000L;)
    {
      String ret = RAKLoRa.rk_recvData();
      if(ret != NULL)
      { 
        DebugSerial.println(ret);
      }
      if((ret.indexOf("OK")>0)||(ret.indexOf("ERROR")>0))
      {
        DebugSerial.println(F("Go to Sleep."));
        RAKLoRa.rk_sleep(1);  //Set RAK811 enter sleep mode
        delay(10000);  //delay 10s
        RAKLoRa.rk_sleep(0);  //Wakeup RAK811 from sleep mode
        break;
      }
    }
  }
}
