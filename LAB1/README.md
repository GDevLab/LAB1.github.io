# บันทึกการทำงาน

**คอนเซ็ปแรก**

- ระบุตัวอุปกรณ์,ชนิด และการทำงาน (ลงทะเบียน Device) พร้อมส่งข้อมูลกลับมายัง Server ทันทีที่เชื่อมต่อ Internet ได้ (Flash Memory/EEPROM)
- เลือก WiFi Network ได้ด้วยตัว user เอง (WiFi Manager)
- บันทึกข้อมูลไปยัง Log File (SD Card)
- update code/firmware online (ArduinoOTA)
- เป็น Server Location ได้ (SPIFFS)

## ขั้นตอนการทำงาน

1. เรียกใช้งาน Libraries

- ESP8266WiFi.h By IDE (ESP8266)
- FS.h By IDE (ESP8266)
- EEPROM.h By IDE
- [WiFiManager.h](https://github.com/tzapu/WiFiManager) By tzapu.
- SD.h By IDE
- [ArduinoOTA.h](https://github.com/jandrassy/ArduinoOTA) By andrassy

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
int address_ssid = 11;
int address_pass = 23;
String devid = "XXXXXXXXXX"; // 10 character
String ssid = "XXXXXXXXXXXX"; // 12 character
String pass = "XXXXXXXXXXXXXXX"; // 15 character

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

  Serial.print("Write WiFi SSID at address "); 
  Serial.println(address_ssid);
  Serial.print("");
  for (int s = 0; s < ssid.length(); ++s)
  {
    EEPROM.write(address_ssid + s, ssid[s]);
    Serial.print(ssid[s]); Serial.print("");
  }

  Serial.println("");

  Serial.print("Write WiFi Password at address "); 
  Serial.println(address_pass);
  Serial.print("");
  for (int p = 0; p < pass.length(); p++)
  {
    EEPROM.write(address_pass + p, pass[p]);
    Serial.print(pass[p]); Serial.print("");
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
int devid_address = 0;
int ssid_address = 11;
int pass_address = 23;
int devid_value = 10;
int ssid_value = 12;
int pass_value = 15;

WiFiManager wm;

bool portalRunning = false;
~~~

3. กระบวนการ Setup โปรแกรม

- อ่านข้อมูลจาก EEPROM ถ้ามีข้อมูลให้เก็บไว้ในตัวแปร เพื่อรอการเชื่อมต่อ Internet และแสดงผล Debug ทาง Serial Monitor หากไม่มีข้อมูลใน EEPROM ให้แสดงผล Device Not Register ผ่านทาง Serial Monitor และรอการเชื่อมต่อ

- เมื่อเชื่อมต่อได้แล้วให้ส่งข้อมูล Device ไปยัง Server เพื่อร้องขอการตั่งค่าจากทาง Server

- 

4. กระบวนการทำงานของโปรแกรม
