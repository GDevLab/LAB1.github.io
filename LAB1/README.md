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

## ขั้นตอนการทำงาน

1. เรียกใช้งาน Libraries

- Arduino.h
- ESP8266WiFi.h By IDE (ESP8266)
- FS.h By IDE (ESP8266)
- EEPROM.h By IDE
- [WiFiManager.h](https://github.com/tzapu/WiFiManager) By Tzapu
- SD.h By IDE
- [ArduinoOTA.h](https://github.com/jandrassy/ArduinoOTA) By Andrassy
- [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) By Ayush Sharma

~~~C++
// EEPROM (save defult config)
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
~~~

2. ประกาศตัวแปร Setting ระบบ

- เขียนข้อมูลไปเป็บไว้ใน EEPROM ก่อน โดยข้อมูลจะแบ่งเป็น 3 ชุดได้แก่
  - Device ID 
  - SSID
  - PASSWORD

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

- กำหนดค่าเริ่มต้น

~~~C++

~~~

3. กระบวนการ Setup โปรแกรม

- อ่านข้อมูลจาก EEPROM ถ้ามีข้อมูลให้เก็บไว้ในตัวแปร เพื่อรอการเชื่อมต่อ Internet และแสดงผล Debug ทาง Serial Monitor หากไม่มีข้อมูลใน EEPROM ให้แสดงผล Device Not Register ผ่านทาง Serial Monitor และรอการเชื่อมต่อ

- เมื่อเชื่อมต่อได้แล้วให้ส่งข้อมูล Device ไปยัง Server เพื่อร้องขอการตั่งค่าจากทาง Server

- 

4. กระบวนการทำงานของโปรแกรม
