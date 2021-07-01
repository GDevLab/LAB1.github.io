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
#include <WiFiManager.h>

// SD (save data to log file)
#include <SD.h>

// ArduinoOTA (update code/firmware online)
#include <ArduinoOTA.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// this wifidata (ssid, password)
#include "secrets.h"

// hardware Config
uint8_t pin_led = 2;
bool ledmode = true;

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

// WiFi Status Config
// WiFiClient    wifiClient;  // HTTP
// WiFiSSLClient wifiClientSSL;  // HTTPS
// int status = WL_IDLE_STATUS;

// Variable Test
const short VERSION = 1;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pin_led, OUTPUT);
  
  EEPROM.begin(512);
  
  delay(1000);
  Serial.println("");

  // อ่านค่า EEPROM เก็บไว้ในตัวแปร
  String devid = EEPROM_read(devid_address, devid_value);
  String ssid = EEPROM_read(ssid_address, ssid_value);
  String pass = EEPROM_read(pass_address, pass_value); 

  // Cut Connecting
  WiFi.disconnect();
  delay(500);

  // WiFi Connecting
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(pin_led,0);
    delay(500);
    Serial.print(".");
    digitalWrite(pin_led,1);
    delay(500);
  }
  // Show IP Connected in Serial port
  Serial.print(WiFi.localIP());
               
  if (devid.isEmpty()) { 
    Serial.println("isEmpty");               
    return;
  }                  
  // Show Device ID in Serial port
  if (!devid == 0){
    Serial.println(devid);
  } else {
    Serial.println("No!! Device ID");
  }
  // Show Firmware Version in Serial port
  Serial.print("Sketch version ");
  Serial.println(VERSION);

  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Start OTA update portal
  if (millis() - timeToUpdate > 3000){
    timeToYpdate=millis();
    // get firmware version fron=m server
    strv=getVersion();
    Serial.println(strv);
    strv=getField(strv,2);
    float curVer=strv.toFloat();
    // get the sketch virsion
    strv=getField(firmwareV[1],2);
    float firmwareV.toFloat();
    if(curVer > firmwareV){
      fwUpdateOTA("firmware.bin");
    }
  }
  // End OTA update portal
}

// ----------------- FUNCTION ZONE -----------------
// Function EEPROM_read
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

// Function GET data from webserver
void handleSketchDownload() {
  const char* SERVER = "www.my-hostname.it";  // Set your correct hostname
  const unsigned short SERVER_PORT = 443;     // Commonly 80 (HTTP) | 443 (HTTPS)
  const char* PATH = "/update-v%d.bin";       // Set the URI to the .bin firmware
  const unsigned long CHECK_INTERVAL = 6000;  // Time interval between update checks (ms)

  // Time interval check
  static unsigned long previousMillis;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < CHECK_INTERVAL)
    return;
  previousMillis = currentMillis;

  // HttpClient client(wifiClient, SERVER, SERVER_PORT);  // HTTP
  HttpClient client(wifiClientSSL, SERVER, SERVER_PORT);  // HTTPS

  char buff[32];
  snprintf(buff, sizeof(buff), PATH, VERSION + 1);

  Serial.print("Check for update file ");
  Serial.println(buff);

  // Make the GET request
  client.get(buff);

  int statusCode = client.responseStatusCode();
  Serial.print("Update status code: ");
  Serial.println(statusCode);
  if (statusCode != 200) {
    client.stop();
    return;
  }

  long length = client.contentLength();
  if (length == HttpClient::kNoContentLengthHeader) {
    client.stop();
    Serial.println("Server didn't provide Content-length header. Can't continue with update.");
    return;
  }
  Serial.print("Server returned update file of size ");
  Serial.print(length);
  Serial.println(" bytes");

  if (!InternalStorage.open(length)) {
    client.stop();
    Serial.println("There is not enough space to store the update. Can't continue with update.");
    return;
  }
  byte b;
  while (length > 0) {
    if (!client.readBytes(&b, 1)) // reading a byte with timeout
      break;
    InternalStorage.write(b);
    length--;
  }
  InternalStorage.close();
  client.stop();
  if (length > 0) {
    Serial.print("Timeout downloading update file at ");
    Serial.print(length);
    Serial.println(" bytes. Can't continue with update.");
    return;
  }

  Serial.println("Sketch update apply and reset.");
  Serial.flush();
  InternalStorage.apply(); // this doesn't return
}
