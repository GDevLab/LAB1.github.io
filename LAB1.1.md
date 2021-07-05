# บันทึกการทำงาน

**ในส่วนนี้จะเป็นการอัปโหลด Fiile System (LittleFS) ไปยัง ESP8266 โดยใช้ AsyncElegantOTA**

ในการกระทำนี้เราจำเป็นต้องมีปลั๊กอิน ESP8266 LittleFS Uploader ใน Arduino IDE ด้วย download และติดตั้ง PlugIn โดยสามารถดูได้จาก [ที่นี่](https://github.com/GridsNodeMCU/nodemcu-LittleFS)

ลองนึกภาพสถานการณ์ที่คุณต้องอัปโหลดไฟล์ไปยังระบบไฟล์ ESP8266 เช่น ไฟล์การกำหนดค่า ไฟล์ HTML, CSS และ JavaScript เพื่ออัปเดตหน้าเว็บเซิร์ฟเวอร์หรือไฟล์อื่นๆ ที่คุณอาจต้องการบันทึกใน LittleFS ผ่าน OTA

เพื่อแสดงวิธีการทำเช่นนี้ เราจะสร้างเว็บเซิร์ฟเวอร์ใหม่ที่ให้บริการไฟล์จากระบบไฟล์ (LittleFS): ไฟล์ HTML, CSS และ JavaScript เพื่อสร้างหน้าเว็บที่ควบคุม ESP8266 GPIO จากระยะไกล

- ก่อนดำเนินการต่อ ตรวจสอบให้แน่ใจว่าคุณได้ติดตั้งไลบรารี `Arduino_JSON` By Arduino เวอร์ชัน 0.1.0 แล้ว 

**_Note_** คุณสามารถติดตั้งไลบรารีนี้ใน Arduino IDE Library Manager เพียงไปที่  Sketch > Include Library > Manage Libraries  และค้นหาชื่อไลบรารีดังนี้ : Arduino_JSON

จากนั้น Copy Code นี้ไปวางใน Arduino IDE

~~~C++
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

#include "LittleFS.h"

// this wifidata (ssid, password)
#include "secrets.h"

// Replace with your network credentials
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {2, 4, 12, 14};

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  }
  initLittleFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });

  server.serveStatic("/", LittleFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  
  // Start server
  server.begin();
}

void loop() {
  AsyncElegantOTA.loop();
  ws.cleanupClients();
}
~~~

- ส่งออกไฟล์ .bin จาก Sketch นี้ จากนั้นไปที่ที่อยู่ IP ของ ESP8266 ตามด้วย /update และอัปโหลดเฟิร์มแวร์นี้ขึ้นไปใหม่

**อัปเดตระบบไฟล์**

ใต้โฟลเดอร์โครงการสร้างโฟลเดอร์ชื่อ "data" และวางไฟล์ HTML, CSS และ JavaScript ต่อไปนี้ (คลิกที่ลิงค์เพื่อดาวน์โหลดไฟล์) ลงในโฟลเดอร์ "data"

- HTML: index.html
- CSS: style.css
- JavaScript: script.js

**_Tip_** เพื่อหาโฟลเดอร์โครงการของคุณ คุณสามารถไปที่ Sketch > Show Sketch Folder

