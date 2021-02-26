//#include <ArduinoJson.h>
//
//DynamicJsonDocument doc(1024);
//
//String text_to_hex(String json)
//{
//  String hex;
//  for(int i=0;i<json.length(); i++){
//    hex.concat(String((int)json[i], HEX));
//  }
////  Serial.print("The CONVERTED STRING IS ");
//  return hex;
//}
//
//
//void setup() {
//  Serial.begin(9600);
//  int i = 0;
//  doc["name"] = "Maheswaran";
//  doc["addr"] = "Goa";
//  doc["gender"] = "Male";
//  String json;
//  serializeJson(doc, json);
//  Serial.println(json);
////  Serial.println(json.length());
//  String hexvalue = text_to_hex(json);
//  Serial.println(hexvalue);
//  char buffer[(int)json.length() * 2 + 1];
//  hexvalue.toCharArray(buffer, (int)json.length() * 2 + 1);
//  Serial.print("hexvalue is ");
//  Serial.println(buffer);
//}
//
//void loop() {
//
//}

struct JSON {
  float t1, t2;
};


void struct_to_string(struct JSON j1){
  String quotes = "\"";
  String jsonStart = "{\"";
  String jsonEnd = "\"}";
  jsonStart += "temperature";
  jsonStart += quotes;
  jsonStart += ":";
  jsonStart += quotes;
  jsonStart += j1.t1;
  jsonStart += jsonEnd;
  Serial.println(jsonStart);
}


void setup(){
  Serial.begin(9600);
  struct JSON j1;
  j1.t1 = 12.3;
  j1.t2 = 14.5;
  struct_to_string(j1);
}
void loop(){
  
}
