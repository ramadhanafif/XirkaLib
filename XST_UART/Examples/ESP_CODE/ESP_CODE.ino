/********************************************************
 * Pengiriman Data ke Server 
 * ESP menerima data melalui UART
 * data diterima dalam bentuk String, menyimpan data json
 *******************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Smart Satelit";
const char* password = "SaTeLiT@30";
StaticJsonDocument<300> doc;
HTTPClient http;//Declare object of class HTTPClient

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  delay(1);
}

void loop() {
  // Read UART from FPGA
  while (!Serial.available());
  String request  = Serial.readStringUntil('\0');
  
  String login = {"username=admin&password=bandung123"};
  
  http.begin("http://192.168.2.7:3000/login");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(login);   //Send the request
  String payload = http.getString();   //Get the response payload
  Serial.println(payload);
  http.end();
  
  DeserializationError error = deserializeJson(doc, payload);
  String token = doc["token"];
  Serial.println(token);

  Serial.println(request);
  http.begin("http://192.168.2.7:3000/card");      //Specify request destination
  http.addHeader("x-access-token", token);
  http.addHeader("Content-Type", "application/json");  //Specify content-type header

  int httpCode1 = http.POST(request);   //Send the request
  String payload1 = http.getString();   //Get the response payload

  Serial.println(httpCode1);   //Print HTTP return code
  Serial.println(payload1);    //Print request response payload

  http.end();  //Close connection
}
