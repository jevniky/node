#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#include "config.h"
#include "functions.h"

void setup()
{
  hardware_init();
  setup_access_point();
  wifi_connect();
  setup_mqtt();
} // setup()

void loop()
{
  server.handleClient();
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval && WiFi.status() != WL_CONNECTED )
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
  }
}
