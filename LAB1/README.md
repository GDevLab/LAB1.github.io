# บันทึกการทำงาน

**คอนเซ็ปแรก**

- ระบุตัวอุปกรณ์,ชนิด และการทำงาน (ลงทะเบียน Device) พร้อมส่งข้อมูลกลับมายัง Server ทันทีที่เชื่อมต่อ Internet ได้ (Flash Memory/EEPROM)
- เลือก WiFi Network ได้ด้วยตัว user เอง (WiFi Manager)
- บันทึกข้อมูลไปยัง Log File (SD Card)
- update code/firmware online (ArduinoOTA, AsyncElegantOTA)
- เป็น Server Location ได้ (SPIFFS)

ข้อมูล

[ESP8266 OTA Updates with ArduinoIDE (OverTheAir)](https://randomnerdtutorials.com/esp8266-ota-updates-with-arduino-ide-over-the-air/)

[ESP8266 OTA Over Tthe Air Arduino](https://randomnerdtutorials.com/esp8266-nodemcu-ota-over-the-air-arduino/)

[EP27. ไม่ต้องใช้สาย ESP32 Over The Air programming](https://www.youtube.com/watch?v=isf_nDJ8wcw)

[EP28 Arduino ESP32 Update Firmware Over The Internet](https://www.youtube.com/watch?v=aigU0d9c55A)

## ขั้นตอนการทำงาน Create Server ด้วย ESP8266

1. เขียนข้อมูลไปเก็บไว้ใน EEPROM ก่อน

- เขียนข้อมูลไปเป็บไว้ใน EEPROM ก่อน โดยข้อมูลจะเป็น Device ID 

~~~C++
#include <EEPROM.h>

int address_devid = 0;
String devid = "Device ID"; // 10 character (Device ID)

#define RESET_EEPROM false

int address = 0;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  Serial.println("");

  Serial.print("Write Device ID at address "); 
  Serial.println(address_devid);
  Serial.print("");
  for (int u = 0; u < devid.length(); ++u)
  {
    EEPROM.write(address_devid + u, devid[u]);
    Serial.print(devid[u]); Serial.print("");
  }

  Serial.println("");

  if (EEPROM.commit()) {
    Serial.println("Data successfully committed");
  } else {
    Serial.println("ERROR! Data commit failed");
  }
}

void loop() {

}
~~~

2. เรียกใช้งาน Libraries และ กำหนดค่าเริ่มต้นให้กับโปรแกรม

**Libraries ที่จำเป็น**

- Arduino.h
- ESP8266WiFi.h By IDE (ESP8266)
- FS.h By IDE (ESP8266)
- EEPROM.h By IDE
- [WiFiManager.h](https://github.com/tzapu/WiFiManager) By Tzapu
- SD.h By IDE
- [ArduinoOTA.h](https://github.com/jandrassy/ArduinoOTA) By Andrassy
- [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) By Ayush Sharma

- เรียกใช้งาน Libraries

~~~C++
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
~~~

- กำหนดค่าเริ่มต้น
  - เรียกใช้ ไฟล์ `"secrets.h"`

~~~C++
// this wifidata (ssid, password)
#include "secrets.h"
~~~

  - กำหนดค่า pin hardware

~~~C++
// hardware Config
uint8_t pin_led = 2;
bool ledmode = true;
~~~

  - กำหนด Version ของ Firmware (ใช้เป็นข้อมูลทดสอบ)

~~~C++
// Firmware Version
const short vers = 1;
~~~

  - กำหนด address ของข้อมูลใน EEPROM

~~~C++
// EEPROM Config
int devid_address = 0;
int devid_value = 10;
~~~

  - ดึงข้อมูล ssid/password WiFi จาก ไฟล์ `"secrets.h"` มาก็บไว้ในตัวแปร ssid, pass

~~~C++
// Loaded from secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
~~~

  - สร้าง Object `AsyncWebServer` บนพอร์ต 80

~~~C++
// WebServer
AsyncWebServer server(80);
~~~

3. กระบวนการ `Setup` โปรแกรม

~~~C++
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  EEPROM.begin(512);
  
  delay(1000);
  Serial.println("");
~~~

- อ่านข้อมูลจาก EEPROM ถ้ามีข้อมูลให้เก็บไว้ในตัวแปร เพื่อรอการเชื่อมต่อ Internet 

~~~C++
  // อ่านค่า EEPROM เก็บไว้ในตัวแปร
  String devid = EEPROM_read(devid_address, devid_value);
~~~

- เชื่อมต่อ Internet และแสดงผลการเชื่อมต่อ Internet และ Debug ออกทาง Serial Monitor

~~~C++
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
~~~

- หลังจากเชื่อมต่อ Internet ได้แล้ว หากมีข้อมูลให้แสดงผล `devid` ผ่านทาง Serial Monitor หากไม่มีข้อมูลใน EEPROM ให้แสดงผล `"No!! Device ID"` ผ่านทาง Serial Monitor

~~~C++
  // Show Device ID in Serial port
  if (!devid == 0){
    Serial.println(devid);
  } else {
    Serial.println("No!! Device ID");
  }
  // Show Firmware Version in Serial port
  Serial.print("Sketch version ");
  Serial.println(vers);
  Serial.println(ssid);
  Serial.println(pass);
~~~

- หากมีการร้องขอข้อมูลเข้ามาจากเบราเซอร์ ESP จะส่งข้อมูล `"Hi! I am ESP8266."` ออกทางหน้าเว็บ

~~~C++
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP8266.");
  });
~~~

**_Tip_** : เพิ่ม ไฟล์หน้า html โดยการเพิ่มการ request ต่อจาก `(/)`

- เริ่ม ElegantOTA

~~~C++
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
~~~

- เริ่มต้นเซิร์ฟเวอร์

~~~C++
  server.begin();
  Serial.println("HTTP server started");

  delay(500);
}
~~~

4. กระบวนการทำงานของโปรแกรม

- `AsyncElegantOTA.loop()` วนรอการร้องขอข้อมูลจาก Client

~~~C++
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
~~~

5. Function Zone

~~~C++
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
~~~

### ทดสอบผลการทำงาน

1. เมื่อเรียกใช้งานเบราเซอร์โดยการป้อนที่อยู่ IP ที่ได้จากการแจก IP ของ Router ควรพบกับคำว่า `"Hi! I am ESP8266."`
2. เมื่อเพิ่มคำว่า `"/update"` ต่อท้ายที่อยู่ IP (ตัวอย่าง : http://192.168.5.122/update) ควรปรากฎหน้า update ElegantOTA

![rfmN9WyXdEU](https://user-images.githubusercontent.com/83551564/124417820-39cba100-dd84-11eb-80e4-1313ae150dbb.png)

## การใช้งาน อัปโหลด "อัปเดตเฟิร์มแวร์ใหม่" ไปยัง ESP8266 ด้วย OTA (Over-the-Air)

ทุกไฟล์ที่อัปโหลดผ่าน OTA ควรอยู่ในรูปแบบ `.bin` คุณสามารถสร้างไฟล์ `.bin` จากไฟล์ Sketch.ino โดยใช้ Arduino IDE

กับร่างของคุณเปิดคุณต้องไปที่ร่าง > ส่งออกรวบรวมไบนารี ถังขยะไฟล์จะถูกสร้างขึ้นจากร่างของคุณ ไฟล์ที่สร้างขึ้นจะถูกบันทึกไว้ในโฟลเดอร์โครงการของคุณ

นั่นคือไฟล์.bin ที่คุณควรอัปโหลดโดยใช้หน้าเว็บ AsyncElegantOTA หากคุณต้องการอัปโหลดเฟิร์มแวร์ใหม่

อัปโหลดร่างเว็บเซิร์ฟเวอร์ใหม่ – ตัวอย่าง
มาดูตัวอย่างการใช้งานจริงกัน ลองนึกภาพว่าหลังจากอัปโหลดภาพร่างก่อนหน้านี้แล้ว คุณต้องการอัปโหลดภาพร่างใหม่ที่ให้คุณควบคุม LED ผ่านเว็บอินเทอร์เฟซเช่นโครงการนี้ นี่คือขั้นตอนที่คุณต้องปฏิบัติตาม:

1. คัดลอกโค้ดต่อไปนี้ไปยัง Arduino IDE ของคุณ อย่าลืมใส่ข้อมูลรับรองเครือข่ายของคุณ



![javaw_tcdteEwJmH](https://user-images.githubusercontent.com/83551564/124419505-d2175500-dd87-11eb-9995-5940494db3df.png)