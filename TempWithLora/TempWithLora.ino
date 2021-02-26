/*
 Author: Maheswaran T Parameswaran
*/

// JSONLibrary Setup
#include <ArduinoJson.h>
DynamicJsonDocument doc(1024);

// DHT Setup
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT DHTSensor(DHTPIN, DHTTYPE);

// LoRa Setup
#include <RAK811.h>
#include <SoftwareSerial.h>
#define WORK_MODE LoRaWAN
#define JOIN_MODE ABP
#define TXpin 11
#define RXpin 10
#define DebugSerial Serial
String NwkSKey = "438991FE0B6B0856799AF47ABF82E0F1";
String AppSKey = "A10D2742058D5F1D6E539AF79A60FDDE";
String DevAddr = "260410C5";
SoftwareSerial ATSerial(RXpin, TXpin);

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial, DebugSerial);

void LoRaSetup(){
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

void DHTSetup(){
  DHTSensor.begin();
}

// custom functions declarations
String getHexFromJson(String JSONString);
DynamicJsonDocument getHumidityAndTemperature();
void sendToThingsNetwork(String hexValue);

void setup()
{
  // LoRa Setup
  LoRaSetup();
  
  // DHT Sensor Setup
  DHTSetup();
}

void loop()
{
  // get JSON Object
  DynamicJsonDocument doc = getHumidityAndTemperature();
  String JSONString;
  serializeJson(doc, JSONString);
  String HEXValue = getHexFromJson(JSONString);
  sendToThingsNetwork(HEXValue);
  
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

DynamicJsonDocument getHumidityAndTemperature(){
  delay(2000);
  doc["humidity"] = DHTSensor.readHumidity();
  doc["temperature"] = DHTSensor.readTemperature();

  return doc;
}

String getHexFromJson(String JSONString){
  String hexValue;
  for(int i=0; i<JSONString.length(); i++){
    hexValue.concat(String((int)JSONString[i], HEX));
  }
  return hexValue;
}

void sendToThingsNetwork(String hexValue){
  DebugSerial.println(F("Start send data..."));
  char buffer[(int)hexValue.length()];
  hexValue.toCharArray(buffer, hexValue.length());
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
