#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"
#include "functions.h"

void setup()
{
  hardware_init();
  setup_access_point();
  wifi_connect();
  setup_mqtt();
  sensors.begin();
} // setup()

void loop()
{
  server.handleClient();

  currentMillisLed = millis();
  if (currentMillisLed - previousMillisLed >= intervalLed && WiFi.status() != WL_CONNECTED )
  {
    previousMillisLed = currentMillisLed;
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
  }

  if (!mqttClient.connected() && WL_CONNECTED == WiFi.status())
  {
    mqtt_connect();
  }

  currentMillisTemp = millis();
  if (currentMillisTemp - previousMillisTemp >= intervalTemp && WL_CONNECTED == WiFi.status() )
  {
    previousMillisTemp = currentMillisTemp;
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.print("Temperature is: ");
    Serial.println(sensors.getTempCByIndex(0));
    temp = sensors.getTempCByIndex(0);
    dtostrf(temp, 5, 2, tempBuff);
    mqttClient.publish(outputTopic, tempBuff);
  }
  mqttClient.loop();
}
