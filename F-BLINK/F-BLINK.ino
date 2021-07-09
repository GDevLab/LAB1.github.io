#include <Arduino.h>

#define LED_BUILTIN 2

#define BLINK_SHARE_FOUND    1
#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 3
#define BLINK_RESET_DEVICE   5

void setup() {
  // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);

    blink(BLINK_SHARE_FOUND);
    delay(2000);
    blink(BLINK_SETUP_COMPLETE);
    delay(2000);
    blink(BLINK_CLIENT_CONNECT);
    delay(2000);
    blink(BLINK_RESET_DEVICE);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void blink(uint8_t count, uint8_t pin = LED_BUILTIN) {
  uint8_t state = HIGH;

  for (int x = 0; x < (count << 1); ++x) {
    digitalWrite(pin, state ^= HIGH);
    delay(50);
  }
}
