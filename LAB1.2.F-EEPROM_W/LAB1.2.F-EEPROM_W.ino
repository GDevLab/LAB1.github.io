#include <Arduino.h>
#include <EEPROM.h>

#include "deviceID.h"

// EEPROM Config
int devid_add = 0;
char data[] = DEVICE_ID;
char* val = data;
// Count char data
char* devID;
char len[10];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(500);
  Serial.println("");

  // Write Data
  EEPROM_write(devid_add, val);

  // Get Data
  devID = EEPROM.get(devid_add, len);

  Serial.print("SAVE DEVICE ID : ");
  Serial.println(devID);
  Serial.println(" : DONE");
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// Function EEPROM_read use Device ID
int EEPROM_write(int index, String text) {
    for (int i = index; i < text.length() + index; ++i) {
        EEPROM.write(i, text[i - index]);
    }
    EEPROM.write(index + text.length(), 0);
    EEPROM.commit();

    return text.length();
}
