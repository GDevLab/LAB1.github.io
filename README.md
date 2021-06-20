# LAB1.GITHUB.IO
  กำหนดค่า DeviceID Online & Offline
  
## ความตั้งใจของโครงการ

ในการพัฒนาอุปกรณ์ IoT ต่างๆ เราอาจมีการทำ Network เพื่อสื่อสารระหว่าง อุปกรณ์ กับ อุปกรณ์ และเมื่ออุปกรณ์มีจำนวนที่มากขึ้นทำให้ยากในการจดจำและการใช้งานสำหรับ ผู้ใช้ ดังนั้นจึงมีแนวคิดว่า หากอุปกรณ์สามารถลงทะเบียนตัวเองเข้ากับ DataCenter หรือ Cousole ที่ทำหน้าที่ควบคุมอุปกรณ์นั้นๆได้ จะทำให้สามารถทราบรายละเอียดของอุปกรณ์ และแยกประเภทการใช้งานให้มีระเบียบมากยิ่งขึ้น พร้อมทั้งยังสามารถควบคุมและเก็บบันทึกข้อมูลใด้จากในระยะไกล และง่ายสำหรับผู้ใช้

## อุปกรณ์
- LED 2 ตัว
- R1K 2 ตัว
- NodeMCU ESP8266 
- RaspberryPi/Desktop

## ขั้นตอนการทำโครงการ

ในการทดลองนี้ ผมจะให้ NodeMCU ลงทะเบียนอุปกรณ์ และมี DeviceID เก็บข้อมูลไว้ใน EEPROM พร้อมเรียกใช้งานเพื่อระบุตัวตนของอุปกรณ์นั้นๆ

**อ้างอิง**
- [nodemcu-EEPROM](https://github.com/GridsNodeMCU/nodemcu-EEPROM)
- [nodemcu-WiFiManager](https://github.com/GridsNodeMCU/nodemcu-WiFiManager)

### กระบวนการทำงานของโปรแกรม

**กำหนดค่าการทำงานของอุปกรณ์**

- เรียกใช้งาน Libraries
  - EEPROM (มาพร้อมกับ Board NodeMCU 1.0 ESP-12E module)
  - ESP8266WiFi, DNSServer, ESP8266WebServer (มาพร้อมกับ Board NodeMCU 1.0 ESP-12E module)
  - [WiFiManager](https://github.com/tzapu/WiFiManager)

~~~C++
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
// https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>
~~~

- กำหนดตำแหน่ง address ให้กับ EEPROM 

~~~C++
// Set EEPROM address
int address = 0;
~~~

- กำหนดค่า web server

~~~C++
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
~~~

- กำหนดสถานะ เริ่มต้น ให้กับอุปกรณ์ LED

~~~C++
// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";
~~~

- กำหนดค่า pin output ให้กับอุปกรณ์ LED

~~~C++
// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;
~~~

**กระบวนการเริ่มต้นทำงานของโปรแกรม void setup()**

- กำหนดให้ใช้พอร์ตสื่อสาร 115200 ผ่านทาง Serial Mornitor

~~~C++
Serial.begin(115200);
  delay(100);
~~~

- เริ่มต้นใช้งาน EEPROM

~~~C++
  // Set start EEPROM
  EEPROM.begin(512);
~~~
  
- เริ่มต้นใช้งาน pinMode output กำหนดค่าให้เป็น digitalWrite มีสถานะ LOW ทั้ง GPIO4, GPIO5
  
~~~C++  
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
~~~

- เริ่มต้นใช้งาน WiFiManager กำหนดค่าภายใน ในการเชื่อมต่อครั้งแรก (ไม่เก็บค่า)

~~~C++
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
~~~

- ดึงค่า ssid และ pass จาก eeprom และทำการเชื่อมต่อ หากไม่ได้เชื่อมต่ออยู่ให้ทำตัวเองให้เป็น access point แสดงผลในรายชื่อ WiFi-SSID ว่า "AutoConnectAP" จากนั้นเข้าสู่การ วนรอบ [`void loop()`] เพื่อรอการกำหนดค่า

~~~C++
  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
~~~

- เมื่อผู้ใช้กำหนดค่าเสร็จ และสามารถ connect Network ได้ให้ประกาศข้อความ Connected เพื่อให้ทราบสถานะการทำงาน (Re-Check การทำงาน) 

~~~C++
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
~~~

- เริ่มต้นกระบวนการการทำงานของ WebServer

~~~C++
  server.begin();
~~~

**กระบวนการวนซ้ำของโปรแกรม void loop()**





**Function การทำงานเพิ่มเติม**

- Function Read/Write EEPROM
  - เรียกใช้ `EEPROM_read` สำหรับการอ่าน
  - เรียกใช้ `EEPROM_write` สำหรับการเขียน

~~~C++
// -----------------FUNCTION EEPROM-----------------
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
 
int EEPROM_write(int index, String text) {
  for (int i = index; i < text.length() + index; ++i) {
    EEPROM.write(i, text[i - index]);
  }
  EEPROM.write(index + text.length(), 0);
  EEPROM.commit();
 
  return text.length() + 1;
}
// -------------------------------------------------
~~~






