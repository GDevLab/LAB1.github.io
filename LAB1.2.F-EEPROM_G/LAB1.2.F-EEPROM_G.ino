#include <Arduino.h>
#include <EEPROM.h>

// EEPROM Config
char val[10];
char* devID;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(500);
  Serial.println("");

  devID = EEPROM.get(0, val);

  Serial.print("in Setup : ");
  Serial.println(devID);
  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("in loop : ");
  Serial.println(devID);
  delay(5000);
}
