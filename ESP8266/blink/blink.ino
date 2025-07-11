#include <ESP8266WiFi.h>

void setup() {
  // LED setup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // LED off (inverted)
}

void loop() {
  // Blink the built-in LED
  digitalWrite(LED_BUILTIN, LOW);  // ON
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH); // OFF
  delay(500);
}
