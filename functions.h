void wifi_connect(char *ssid_conn, char *pass_conn);
void wifi_event(WiFiEvent_t event);
void handleRoot(void);
void setup_access_point(void);
void eeprom_read(int start_address, char readBuff[]);
void eeprom_write(char *writeData, int address);
void eeprom_clear(void);
void hardware_init(void);
void buffer_clear(char clearBuff[], int buffSize);
void led_wifi_on(void);
void led_wifi_off(void);
void led_ap_on(void);
void led_ap_off(void);

void wifi_connect(char *ssid_conn, char *pass_conn)
{
  WiFi.mode(WIFI_STA);
  // delete old config
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(wifi_event);
  WiFi.begin(ssid_conn, pass_conn);
  while (WiFi.status() != WL_CONNECTED) {
#if DEBUG
    Serial.print(F("."));
#endif
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
    delay(500);
    if ( WIFI_TIMEOUT > wifiTimeoutConn ) {
      wifiTimeoutConn++;
              #if DEBUG
        Serial.print(F("wifiTimeout (in wifi_connect): "));
        Serial.println(wifiTimeoutConn);
#endif
    } else {
      wifiTimeoutConn = 0; // Clear wifi timeout for next use
#if DEBUG
      Serial.println(F("Time out. Failed to connect."));
#endif
      led_wifi_off();
      break; // break out of this loop and continue with AP setup
    }
  } // while (WiFi.status() != WL_CONNECTED)
}

void wifi_event(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
#if DEBUG
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
#endif
      led_wifi_on();
      handleClientFlag = 0; // So the client handle loop DOESNT start
      wifiTimeoutEvent = 0; // Clear wifi timeout for next use
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
#if DEBUG
      Serial.println("WiFi lost connection");
#endif
      if (WIFI_TIMEOUT > wifiTimeoutEvent)
      {
        wifiTimeoutEvent++;
        #if DEBUG
        Serial.print(F("wifiTimeout (in wifi_event): "));
        Serial.println(wifiTimeoutEvent);
#endif
      }
      else {
        wifiTimeoutEvent = 0; // Clear wifi timeout for next use
        handleClientFlag = 1; // So the client handle loop starts
#if DEBUG
        Serial.println(F("Time out. Failed to connect in wifi_event."));
#endif
#if DEBUG
        Serial.print(F("\nRestarting the node in wifi_event.\n"));
#endif
        ESP.restart\();
      }
      digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
      break;
  }
}

void handleRoot(void)
{
  if (server.hasArg("ssid"))
  {
    String ssid_web = server.arg("ssid");
    ssid_web.toCharArray(ssid, 32);
#if DEBUG
    Serial.print(F("SSID WEB ["));
    Serial.print(ssid);
    Serial.println(F("]"));
#endif
    eeprom_clear();
#if DEBUG
    Serial.print(F("SSID to EEPROM ["));
    Serial.print(ssid);
    Serial.println(F("]"));
#endif
    eeprom_write(ssid, SSID_ADDR_START);
    if (server.hasArg("pass"))
    {
      String pass_web = server.arg("pass");
      pass_web.toCharArray(pass, 32);
#if DEBUG
      Serial.print(F("PASS WEB ["));
      Serial.print(pass);
      Serial.println(F("]"));
#endif
      server.send(200, "text/html", htmlBodyThx);
#if DEBUG
      Serial.print(F("PASS to EEPROM ["));
      Serial.print(pass);
      Serial.println(F("]"));
#endif
      eeprom_write(pass, PASS_ADDR_START);
    }
#if DEBUG
    Serial.print(F("\nAttempting to connect again.\n"));
#endif
    wifi_connect(ssid, pass);
  }
  else
  {
    server.send(200, "text/html", htmlBody);
  }
}

//******************************************************************************
// SETUP ACCESS POINT
// Name: setup_access_point()
// Desc: Setups access point and server.
// Input: void
// Return: void
//******************************************************************************
void setup_access_point(void)
{
  // delete old config
  // WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ACCESS_POINT_NAME, ACCESS_POINT_PASS);
  IPAddress apip = WiFi.softAPIP();
#if DEBUG
  Serial.print("visit: ");
  Serial.println(apip);
#endif
  server.on("/", handleRoot);
  server.begin();
#if DEBUG
  Serial.println(F("Server started"));
#endif
}

//******************************************************************************
// EEPROM READ
// Name: eeprom_read()
// Desc: Reads data from EEPROM.
// Input: address where to start reading, buffer where to save this reaq data
// Return: Buffer with read data
//******************************************************************************
void eeprom_read(int start_address, char readBuff[])
{
  //  static char buff[32] = ""; // buffer where to store read value
  int i = 0; // array index counter
  int eepromRead = !END_OF_EEPROM_READ;
  while ( END_OF_EEPROM_READ != eepromRead ) {
    eepromRead = EEPROM.read(start_address);
    if ( '\0' != char(eepromRead) ) {
      readBuff[i] = char(eepromRead);
      i++;
    }
    start_address = start_address + 1; // advance to the next address of the EEPROM
    delay(100);
  }
}

//******************************************************************************
// EEPROM WRITE
// Name: eeprom_write()
// Desc: Writes data to EEPROM.
// Input: data to write (as char array), address where to write
// Return: void
//******************************************************************************
void eeprom_write(char *writeData, int address)
{
#if DEBUG
  Serial.print(F("Writting ["));
  Serial.print(writeData);
  Serial.print(F("] to EEPROM with size of "));
  Serial.println(strlen(writeData));
#endif
  for (int i = 0; i < strlen(writeData); i++)
  {
#if DEBUG
    Serial.print(F("Writing "));
    Serial.print(writeData[i]);
    Serial.print(F(" to address "));
    Serial.println(address);
#endif
    EEPROM.write(address, writeData[i]);
    address++;
    delay(100);
  }
#if MICROCONTROLLER == ESP8266
  EEPROM.commit(); // Commit changes
#endif
#if DEBUG
  Serial.println(F("Changes commited to EEPROM."));
#endif
#if DEBUG
  Serial.println(F("Writing done."));
#endif
}

//******************************************************************************
// EEPROM CLEAR
// Name: eeprom_clear()
// Desc: Clear EEPROM.
// Input: void
// Return: void
//******************************************************************************
void eeprom_clear(void)
{
#if DEBUG
  Serial.println(F("Clearing EEPROM."));
#endif
  for (int i = 0 ; i < 64; i++)
  {
    EEPROM.write(i, 0);
#if DEBUG
    Serial.print(F("."));
    digitalWrite(RED_LED, !digitalRead(RED_LED));
#endif
  }
#if DEBUG
  Serial.println(F("\nEEPROM cleared."));
#endif
}

//******************************************************************************
// HARDWARE INITIALIZATION
// Name: hardware_init()
// Desc: Initialization of hardware. Set up LED outputs, Start EEPROM and serial communication
// Input: void
// Return: void
//******************************************************************************
void hardware_init(void)
{
  delay(2000); // Let the CPU "wake up" a bit. 2s
  WiFi.disconnect(true);
  // Set LEDs pins as outputs
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Turn off the LEDs at start.
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  // turn on the power on LED
  digitalWrite(RED_LED, HIGH);

#if MICROCONTROLLER == ESP8266
  EEPROM.begin(512); // If the uC is ESP type, there is need to call EEPROM.begin()
  delay(500);
#endif
  Serial.begin(9600);
  delay(500);
}


//******************************************************************************
// BUFFER CLEAR
// Name: buffer_clear()
// Desc: Clear any buffer passed to the function
// Input: buffer to clear, size of the buffer
// Return: void
//******************************************************************************
void buffer_clear(char clearBuff[], int buffSize)
{
  for (int i = 0; i <= buffSize; i++)
  {
    clearBuff[i] = '\0';
  }
}

void led_wifi_on(void)
{
  led_ap_off();
  digitalWrite(GREEN_LED, HIGH);
}
void led_wifi_off(void)
{
  digitalWrite(GREEN_LED, LOW);
}
void led_ap_on(void)
{
  led_wifi_off();
  digitalWrite(YELLOW_LED, HIGH);
}
void led_ap_off(void)
{
  digitalWrite(YELLOW_LED, LOW);
}
