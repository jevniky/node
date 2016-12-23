#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "config.h"
#include "functions.h"

void setup()
{
  hardware_init();

  //  eeprom_clear();
  //  eeprom_write("UPC2444789", SSID_ADDR_START);
  //  eeprom_write("JNTECCZZ", PASS_ADDR_START);

  // Look for SSID in the EEPROM
  eeprom_read(SSID_ADDR_START, ssid);
  if ('\0' != ssid) // If the ssid is NOT empty
  {
    // Look for PASS in the EEPROM
    eeprom_read(PASS_ADDR_START, pass);
    if ('\0' != pass) // If the ssid is NOT empty
    {
#if DEBUG
      Serial.print(F("\nSSID: ["));
      Serial.print(ssid);
      Serial.print(F("]\n"));
      Serial.print(F("PASS: ["));
      Serial.print(pass);
      Serial.print(F("]\n"));
#endif
    }
  }

  if ('\0' != ssid) // if the ssid is not empty
  {
    // attempt to connect to Wifi network:
    wifi_connect(ssid, pass);
  }

  // If there was unsuccessful connection to wifi network
  if ( WiFi.status() != WL_CONNECTED )
  {
    setup_access_point();
    server.begin();
    led_ap_on();
    handleClientFlag = 1; // So the client handle loop starts
  }

  while (handleClientFlag)
  {
    server.handleClient();
  }
} // setup()

void loop()
{
  if ( WL_CONNECTED ==  WiFi.status() )
  {
    Serial.println("Simulate loop :P...");
  }
  
}
