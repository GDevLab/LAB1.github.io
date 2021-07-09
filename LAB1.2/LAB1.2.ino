/*------------------------------------------------------------#
* 
*
*
*
#------------------------------------------------------------*/
// EEPROM (save default config Device ID)
#include <Arduino.h>
#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

#include "LittleFS.h"

#include "secrets.h"

#define USE_SERIAL Serial

// hardware Config
bool ledState = 0;
const int pin_led = 2;

// Firmware Version
const char* fwVersion = "1.0.0";
// Firmware Location
const char* fwUrlBase = "http://127.0.0.1/ESP8266UFW/";

// EEPROM Config
int devid_address = 0;
int devid_value = 10;

// Loaded from secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
    EEPROM.begin(512);
    
    delay(1000);
    Serial.println("");

    // Cut Connecting
    WiFi.disconnect();
    delay(500);
    
    Serial.println("Start Connect to Internet");
    conn();

    String david = EEPROM_read(devid_address, devid_value);
    // Show Device ID in Serial port
    if (!david == 0){
      Serial.print(david);
    } else {
      Serial.print("No!! Device ID");
    }
    Serial.println("");

    // Check Firmware Update
    checkForUpdates();
    delay(2000);
}

void loop() {
    Serial.println("in loop"); // for debug
    
    pinMode(pin_led, OUTPUT);
    digitalWrite(pin_led, LOW);
    delay(2000);
    digitalWrite(pin_led, HIGH);
    delay(2000);
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Can't Connected to WiFi"); // for debug
        conn();
    } else {
        Serial.println("Wifi Connected"); // for debug
        WiFiClient client;
        HTTPClient http;
        String david = EEPROM_read(devid_address, devid_value);
        String url = "http://127.0.0.1/ESP8266UFW/add.php?did="+String(david);
        Serial.println(url); // for debug
        // Sent url
        http.begin(client, url);
        http.addHeader("Content-Type", "application/json");
        
        Serial.print("[HTTP] POST...\n"); // for debug
        
        // start connection and send HTTP header and body
        int httpCode = http.POST("{\"hello\":\"world\"}"); // for debug
        
        // httpCode will be negative on error
        if (httpCode > 0) {
          
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK) {
              const String& payload = http.getString();
              Serial.println("received payload:\n<<");
              Serial.println(payload);
              Serial.println(">>");
            }
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
}

// Function Zone
void conn(){
    WiFi.begin(ssid, pass);
    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(1000);
    }
    // Show IP Connected in Serial port
    Serial.println(""); // for debug
    Serial.println(WiFi.localIP()); // for debug
}

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

// Function Check Update Firmware Version
void checkForUpdates() {
    String mac = WiFi.macAddress();
    String fwURL = String( fwUrlBase );
    fwURL.concat( mac );
    String fwVersionURL = fwURL;
    fwVersionURL.concat( ".version" );
  
    Serial.print( "MAC address : " );
    Serial.println( mac );
    Serial.print( "Firmware version URL : " );
    Serial.println( fwVersionURL );
}
