#include <ESP8266WiFi.h>

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

void setup() {
  // put your setup code here, to run once:
    connWiFi(); 
}

void loop() {
  // put your main code here, to run repeatedly:

}

void connWiFi() {
    Serial.println("Connecting to: " + String(SSID));
    WiFi.mode(WIFI_STA); // Setup ESP in client mode
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    
    WiFi.begin(ssid, pass);
  
    int wait_passes = 0;
    
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if (++wait_passes >= 10) {
        WiFi.begin(ssid, pass);
        wait_passes = 0;
      }
    }  
    Serial.println("\nConnected to WiFi!");
    Serial.println("Local IP address: " + WiFi.localIP().toString());
}
