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
#include <WiFiManager.h>
~~~

- กำหนดตำแหน่ง address ให้กับ EEPROM 

~~~C++
int address = 0;
~~~

- กำหนดค่า web server ใช้งาน port 80 และกำหนดค่า header

~~~C++
WiFiServer server(80);

String header;
~~~

- กำหนดสถานะ เริ่มต้น ให้กับอุปกรณ์ LED

~~~C++
String output5State = "off";
String output4State = "off";
~~~

- กำหนดค่า pin output ให้กับอุปกรณ์ LED

~~~C++
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
  EEPROM.begin(512);
~~~
  
- เริ่มต้นใช้งาน pinMode output กำหนดค่าให้เป็น digitalWrite มีสถานะ LOW ทั้ง GPIO4, GPIO5
  
~~~C++  
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
~~~

- เริ่มต้นใช้งาน WiFiManager กำหนดค่าภายใน ในการเชื่อมต่อครั้งแรก (ไม่เก็บค่า)

~~~C++
  WiFiManager wifiManager;
~~~

- ดึงค่า ssid และ pass จาก eeprom และทำการเชื่อมต่อ หากไม่ได้เชื่อมต่ออยู่ให้ทำตัวเองให้เป็น access point แสดงผลในรายชื่อ WiFi-SSID ว่า "AutoConnectAP" จากนั้นเข้าสู่การ วนรอบ [`void loop()`] เพื่อรอการกำหนดค่า

~~~C++
  wifiManager.autoConnect("AutoConnectAP");
~~~

- เมื่อผู้ใช้กำหนดค่าเสร็จและสามารถ connect Network ได้ให้ประกาศข้อความ Connected. เพื่อให้ทราบสถานะของการทำงาน (Re-Check การทำงาน) 

~~~C++
  Serial.println("Connected.");
~~~

- เริ่มต้นกระบวนการการทำงานของ WebServer

~~~C++
  server.begin();
~~~

**กระบวนการวนซ้ำของโปรแกรม void loop()**

- กำหนดให้รอการเชื่อมต่อจาก clients

~~~C++
WiFiClient client = server.available();
~~~

- ถ้ามี client เชื่อมต่อเข้ามาใหม่ ก็ให้แสดงผลออกทาง serial port จากนั้นให้สร้าง String เพื่อเก็บข้อมูลจาก client

~~~C++
  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
~~~

- จากนั้นใช้ while loop วนซ้ำ ถ้ามีข้อมูลที่อ่านได้จาก client ให้แสดงผลออกทาง serial monitor

~~~C++
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);   
~~~

- กำหนดค่า header ถ้าข้อมูลอักขระขึ้นบรรทัดใหม่ ('\n') นั่นคือจุดสิ้นสุดคำขอ HTTP ของไคลเอ็นต์ ดังนั้นให้ส่ง response จากนั้น if เชคต่อ

~~~C++
        header += c;
        if (c == '\n') {
~~~

- ถ้า response code (e.g. HTTP/1.1 200 OK) ก็ให้แสดงผลให้ผู้ใช้ทราบถึงกระบวนการทำงานต่อไป (Re-Check การทำงาน)

~~~C++
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
~~~~

- เปิดและปิด GPIO 
  - ถ้า GET on ก็ให้ digitalWrite output HIGH
  - ถ้า GET off ก็ให้ digitalWrite output LOW

~~~C++
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
~~~

- ส่วนของการแสดงผล HTML web page

~~~C++
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
~~~

- ส่วนของการแสดงผล HTML state buttons

~~~C++
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("</body></html>");
~~~            
            
- แสดงการตอบสนอง HTTP ลงท้ายด้วยบรรทัดว่างอื่น            

~~~C++
            client.println();
            // Break out of the while loop
            break;
~~~

- แต่ถ้ามีการขึ้นบันทัดใหม่ก็ให้ clear currentLine แต่ถ้านอกเหนือจากนั้นก็ให้เพิ่ม end ที่ currentLine

~~~C++
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
~~~

- ล้างค่า header และสิ้นสุดการทำงานของ client   

~~~C++
    header = "";
    
    client.stop();
    
    Serial.println("Client disconnected.");
    Serial.println("");
  }
~~~

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
