/*------------------------------------------------------------#
* 
*
*
*
#------------------------------------------------------------*/
// EEPROM (save default config Device ID)
#include <EEPROM.h>

// WiFiManager (change & connect to network WiFi)
#include <ESP8266WiFi.h>

// SD (save data to log file)
#include <SD.h>

// ArduinoOTA (update code/firmware online)
#include <ArduinoOTA.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

// this wifidata (ssid, password)
#include "secrets.h"

// hardware Config
uint8_t pin_led = 2;
bool ledmode = true;

// Firmware Version
const short vers = 1;

// EEPROM Config
int devid_address = 0;
int devid_value = 10;

// Loaded from secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

// WebServer
AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  EEPROM.begin(512);
  
  delay(1000);
  Serial.println("");

  // อ่านค่า EEPROM เก็บไว้ในตัวแปร
  String devid = EEPROM_read(devid_address, devid_value);

  // Cut Connecting
  WiFi.disconnect();
  delay(500);

  // WiFi Connecting
  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(pin_led,0);
    delay(500);
    Serial.print(".");
    digitalWrite(pin_led,1);
    delay(500);
  }
  // Show IP Connected in Serial port
  Serial.println(WiFi.localIP());
                                
  // Show Device ID in Serial port
  if (!devid == 0){
    Serial.println(devid);
  } else {
    Serial.println("No!! Device ID");
  }
  
  // Show Firmware Version in Serial port
  Serial.print("Sketch version : ");
  Serial.println(vers);
  Serial.println(ssid);
  Serial.println(pass);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP8266.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  delay(500);
}

void loop() {
//  // put your main code here, to run repeatedly:
//  // Start OTA update portal
//  if (millis() - timeToUpdate > 3000){
//    timeToYpdate=millis();
//    // get firmware version fron=m server
//    strv=getVersion();
//    Serial.println(strv);
//    strv=getField(strv,2);
//    float curVer=strv.toFloat();
//    // get the sketch virsion
//    strv=getField(firmwareV[1],2);
//    float firmwareV.toFloat();
//    if(curVer > firmwareV){
//      fwUpdateOTA("firmware.bin");
//    }
//  }
//  // End OTA update portal
AsyncElegantOTA.loop();
}

// ----------------- FUNCTION ZONE -----------------
// Function EEPROM_read use Device ID
String EEPROM_read(int index, int length) {
  String text = "";
  char ch = 1;
 
  for (int i = index; (i < (index + length)) && ch; ++i) {
    if (ch = EEPROM.read(i)) {
      text.concat(ch);
    }
  }
  return text;
}
