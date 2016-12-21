#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "config.h"
#include "functions.h"

void setup() {
  hardware_init();

  //  eeprom_clear();
  //
  //  eeprom_write("UPC2444789", SSID_ADDR_START);
  //  eeprom_write("JNTECCZZ", PASS_ADDR_START);

  ssid = eeprom_read(SSID_ADDR_START, buff);
#if DEBUG
  Serial.print(F("SSID: ["));
  Serial.print(ssid);
  Serial.println(F("]"));
#endif
  buffer_clear(buff, sizeof(buff)); // clear buffer first
  pass = eeprom_read(PASS_ADDR_START, buff);
#if DEBUG
  Serial.print(F("PASS: ["));
  Serial.print(pass);
  Serial.println(F("]"));
#endif

  if ('\0' != ssid) // if the ssid is not empty...
  {
    // attempt to connect to Wifi network:
    wifi_connect(ssid, pass);
  }

  // If there was successful connection to wifi network
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH); // Turn on the green led
#if DEBUG
    Serial.print(F("Connected to "));
    Serial.println(ssid);
#endif
  }
  else if ( WiFi.status() != WL_CONNECTED )
  {
    setup_access_point();
    server.begin();
    digitalWrite(YELLOW_LED, HIGH);
  }

  while(1)
  {
    server.handleClient();
  }
} // setup()

void loop()
{

}
