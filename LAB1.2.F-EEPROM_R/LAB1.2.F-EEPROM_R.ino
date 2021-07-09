#include <Arduino.h>
#include <EEPROM.h>

// EEPROM Config
int devid_address = 0;
int devid_value = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(500);
  Serial.println("");

  String devID = EEPROM_read(devid_address, devid_value);
  Serial.print("in setup : ");
  Serial.println(devID);
  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:
  String devID = EEPROM_read(devid_address, devid_value);
  Serial.print("in loop : ");
  Serial.println(devID);
  delay(5000);
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
