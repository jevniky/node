void wifi_connect(char *ssid_conn, char *pass_conn)
{
  WiFi.begin(ssid_conn, pass_conn);
  while (WiFi.status() != WL_CONNECTED) {
#if DEBUG
    Serial.print(F("."));
#endif
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
    delay(500);
    if ( WIFI_TIMEOUT > wifiTimeout ) {
      wifiTimeout++;
    } else {
      // ToDo: Is this clear necessarry?
      wifiTimeout = 0; // Clear wifi timeout for next use
#if DEBUG
      Serial.println(F("Time out. Failed to connect."));
#endif
      digitalWrite(GREEN_LED, LOW);
      break; // break out of this loop and continue with AP setup
    }
  } // while (WiFi.status() != WL_CONNECTED)
}

void handleRoot(void) {
  if (server.hasArg("ssid"))
  {
    String ssid_web = server.arg("ssid");
    ssid_web.toCharArray(ssid, 32);
    Serial.print(F("SSID WEB ["));
      Serial.print(ssid_web);
      Serial.println(F("]"));
    if (server.hasArg("pass"))
    {
      String pass_web = server.arg("pass");
      pass_web.toCharArray(pass, 32);
      Serial.print(F("PASS WEB ["));
      Serial.print(pass_web);
      Serial.println(F("]"));
      server.send(200, "text/html", htmlBodyThx);
      wifi_connect(ssid, pass);
    }
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
void setup_access_point(void) {
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
char *eeprom_read(int start_address, char readBuff[])
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
  return readBuff;
}

//******************************************************************************
// EEPROM WRITE
// Name: eeprom_write()
// Desc: Writes data to EEPROM.
// Input: data to write (as char array), address where to write
// Return: void
//******************************************************************************
void eeprom_write(char *writeData, int address) {
#if DEBUG
  Serial.print(F("Writting ["));
  Serial.print(writeData);
  Serial.print(F("] to EEPROM with size of "));
  Serial.println(strlen(writeData));
#endif
  for (int i = 0; i < strlen(writeData); i++) {
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
void eeprom_clear(void) {
#if DEBUG
  Serial.println(F("Clearing EEPROM."));
#endif
  for (int i = 0 ; i < 512; i++) {
    EEPROM.write(i, 0);
#if DEBUG
    Serial.print(F("."));
#endif
  }
#if DEBUG
  Serial.println(F("EEPROM cleared."));
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
  for (int i = 0; i <= buffSize; i++) {
    clearBuff[i] = '\0';
  }
}

