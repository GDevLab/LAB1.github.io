#include <EEPROM.h>

// WiFiManager (change & connect to network WiFi)
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

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

// EEPROM Config
int devid_address = 0;
int ssid_address = 11;
int pass_address = 23;
int devid_value = 10;
int ssid_value = 12;
int pass_value = 15;

// WiFi Config
WiFiManager wm;
bool portalRunning = false;

// Loaded from secrets.h
const char MY_SSID[] = SECRET_SSID;
const char MY_PASS[] = SECRET_PASS;

// WebServer
// AsyncWebServer server(80);

// Variable Test
// const short VERSION = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

}
