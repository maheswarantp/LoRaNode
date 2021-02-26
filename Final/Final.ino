/* 
 *  Author: Maheswaran T Parameswaran
 *  Date: 3rd Feb 2021
*/

#include <string.h>

// Sensors Config Section
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT DHTSensor(DHTPIN, DHTTYPE);

# define getName(var, str)  sprintf(str, "%s", #var)  

// LoRaNode Config Section
#include <RAK811.h>
#include <SoftwareSerial.h>
#define WORK_MODE LoRaWAN
#define JOIN_MODE ABP

// connection parameters
String NwkSKey = "438991FE0B6B0856799AF47ABF82E0F1";
String AppSKey = "A10D2742058D5F1D6E539AF79A60FDDE";
String DevAddr = "260410C5";

#define TXPIN 11
#define RXPIN 10
#define DebugSerial Serial
SoftwareSerial ATSerial(RXPIN, TXPIN);
RAK811 RAKLoRa(ATSerial, DebugSerial);

// STRUCT FOR JSON
struct JSONBody {
  String quotes = "\"";
  String jsonStart = "{";
  String jsonEnd = "\"}";
  String comma = ",";
  String colon = ":"; 
};

// CUSTOM FUNCTIONS
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

String text_to_hex(String json)
{
  String hex;
  for(int i=0;i<json.length(); i++){
    hex.concat(String((int)json[i], HEX));
  }
  return hex;
}

String giveKey(String key){
  String returnKey = "";
  returnKey += "\"";
  returnKey += key;
  returnKey += "\"";
  return returnKey;
}

String giveVal(float key){
  String returnVal = "";
  returnVal += "\"";
  returnVal += key;
  returnVal += "\"";
  return returnVal;
}

String struct_to_string(struct JSONBody json_struct, int numberOfSensorElements, String nameOfSensors[], float valueOfSensors[]){
  String json_string = "{";
  for(int i=0; i<numberOfSensorElements; i++){
    json_string += giveKey(nameOfSensors[i]);
    json_string += ":";
    json_string += giveVal(valueOfSensors[i]);
    if(numberOfSensorElements - i > 1){
      json_string += ",";
    }
  }
  json_string += "}";
  return json_string;
}

struct Data {
  float temperature, humidity;
};




void setup(){
  Serial.begin(9600);
  char str[] = "strtok needs to be called several times to split a string";
  int init_size = strlen(str);
  char delim[] = " ";

  char *ptr = strtok(str, delim);
  char str1[10];
  int count = 0;
  while (ptr != NULL)
  {
    str1[count] = ptr;
    count += 1;
//    Serial.println(ptr);
    ptr = strtok(NULL, delim);
  }
  for(int i = 0; i < 10; i++){
    Serial.println(&str1[i]);
  }
}
void loop(){

}
