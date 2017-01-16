void mqtt_connect(void);
void callback(char* topic, byte* payload, unsigned int length);
uint8_t setup_mqtt(void);
uint8_t wifi_connect(void);
void wifi_event(WiFiEvent_t event);
void handle_root(void);
void setup_access_point(void);
void eeprom_read(int start_address, char readBuff[]);
void eeprom_write(char *writeData, int address);
void eeprom_clear(uint8_t from, uint8_t to);
void hardware_init(void);
void buffer_clear(char clearBuff[], int buffSize);
void led_wifi_on(void);
void led_wifi_off(void);
void led_ap_on(void);
void led_ap_off(void);

//******************************************************************************
// MQTT CONNECT
// Name: mqtt_connect()
// Desc: Function for connection to mqtt broker
// Input: void
// Return: void
//******************************************************************************
void mqtt_connect(void)
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    #if DEBUG
    Serial.print("Attempting MQTT connection...");
    #endif
    sprintf(topicName, "sensor/%s", mainTopicChar);
    strcat(topicName, "/state");
    // Attempt to connect
    if (mqttClient.connect(DeviceNameChar, topicName, WILL_QOS, WILL_RETAIN, WILL_MESSAGE))
    {
      IPAddress ip_addr = WiFi.localIP();
      sprintf(ipMessage, "%d.%d.%d.%d", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
      #if DEBUG
      Serial.println("connected");
      #endif
      // Once connected, publish an announcement...
      sprintf(topicName, "sensor/%s", mainTopicChar);
      strcat(topicName, "/state");
      mqttClient.publish(topicName, CLIENT_ON, MESSAGE_RETAIN_ON);

      sprintf(topicName, "sensor/%s", mainTopicChar);
      strcat(topicName, "/info/ip");
      mqttClient.publish(topicName, ipMessage, MESSAGE_RETAIN_ON);

      sprintf(topicName, "sensor/%s", mainTopicChar);
      strcat(topicName, "/info/type");
      mqttClient.publish(topicName, DEVICE_TYPE);
      // ... and resubscribe
      // mqttClient.subscribe("inTopic");
      // prepare output topic
      sprintf(outputTopic, "sensor/%s", mainTopicChar);
      strcat(outputTopic, "/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 3 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}

//******************************************************************************
// CALLBACK
// Name: callback()
// Desc: callback function for mqtt incomming messages
// Input: char* topic, byte* payload, unsigned int length
// Return: void
//******************************************************************************
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//******************************************************************************
// CALLBACK
// Name: setup_mqtt()
// Desc: Setting up the mqtt
// Input: void
// Return: void
//******************************************************************************
uint8_t setup_mqtt(void)
{
  eeprom_read(BROKER_IP_START, broker);
  if ( '\0' != broker )
  {
    mqttClient.setServer(broker, MQTT_BROKER_PORT);
    mqttClient.setCallback(callback);
#if DEBUG
    Serial.print("MQTT broker set up. IP: ");
    Serial.println(broker);
#endif
  }
  String mainTopic =   String(mac[WL_MAC_ADDR_LENGTH - 6], HEX) +
                String(mac[WL_MAC_ADDR_LENGTH - 5], HEX) +
                String(mac[WL_MAC_ADDR_LENGTH - 4], HEX) +
                String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  mainTopic.toUpperCase();
  mainTopic.toCharArray(mainTopicChar, 32);
}

//******************************************************************************
// WIFI CONNECT
// Name: wifi_connect()
// Desc: Function for wifi connection
// Input: char *ssid_conn, char *pass_conn)
// Return: void
//******************************************************************************
uint8_t wifi_connect(void)
{
  // delete old config
  WiFi.disconnect(true);
  delay(1000);
  eeprom_read(SSID_ADDR_START, ssid);
  if ('\0' != ssid) // If the ssid is NOT empty
  {
    // Look for PASS in the EEPROM
    eeprom_read(PASS_ADDR_START, pass);
    if ('\0' != pass) // If the pass is NOT empty
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
    // attempt to connect to Wifi network.
    WiFi.begin(ssid, pass);
  } // if ('\0' != ssid)
}

//******************************************************************************
// WIFI EVENT
// Name: wifi_event()
// Desc: Handles wifi events
// Input: WiFiEvent_t event
// Return: void
//******************************************************************************
void wifi_event(WiFiEvent_t event)
{
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED: // event = 0
#if DEBUG
      Serial.println(F("WiFi connected"));
#endif
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED: // event = 1
#if DEBUG
      // Serial.println(F("WiFi lost connection"));
#endif
      led_wifi_off();
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE: // event = 2
#if DEBUG
      Serial.println(F("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE"));
#endif
      break;
    case WIFI_EVENT_STAMODE_GOT_IP: // event = 3
#if DEBUG
      Serial.println(F("IP address: "));
      Serial.println(WiFi.localIP());
#endif
      led_wifi_on();
      break;
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT: // event = 4
#if DEBUG
      Serial.println(F("WIFI_EVENT_STAMODE_DHCP_TIMEOUT"));
#endif
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED: // event = 5
#if DEBUG
      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STACONNECTED"));
#endif
      led_ap_on();
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED: // event = 6
#if DEBUG
      Serial.println(F("WIFI_EVENT_SOFTAPMODE_STADISCONNECTED"));
#endif
      led_ap_off();
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED: // event = 7
#if DEBUG
      // Serial.println(F("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED"));
#endif
      break;
    default:
#if DEBUG
      Serial.print(F("Other event: "));
      Serial.println(event);
#endif
      break;
  }
}
//******************************************************************************
// HANDLE ROOT
// Name: handle_root()
// Desc: Handles incomming data from web page
// Input: void
// Return: void
//******************************************************************************
void handle_root(void)
{
  if ( server.hasArg("wifi_data") )
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
      eeprom_clear(SSID_ADDR_START, SSID_ADDR_STOP);
      eeprom_write(ssid, SSID_ADDR_START);
    } // if (server.hasArg("ssid"))
    if (server.hasArg("pass"))
    {
      String pass_web = server.arg("pass");
      pass_web.toCharArray(pass, 32);
#if DEBUG
      Serial.print(F("PASS WEB ["));
      Serial.print(pass);
      Serial.println(F("]"));
#endif
      eeprom_clear(PASS_ADDR_START, PASS_ADDR_STOP);
      eeprom_write(pass, PASS_ADDR_START);
    } // if (server.hasArg("pass"))
    server.send(200, "text/html", htmlBodyThx);
#if DEBUG
    Serial.print(F("\nAttempting to connect again.\n"));
#endif
    wifi_connect();
  } // if ( server.hasArg("wifi_data") )
  if ( server.hasArg("broker_data") )
  {
    if (server.hasArg("broker"))
    {
      String broker_web = server.arg("broker");
      broker_web.toCharArray(brokerChar, 16);
      eeprom_clear(BROKER_IP_START, BROKER_IP_STOP);
#if DEBUG
      Serial.print(F("Broker from web: "));
      Serial.println(brokerChar);
#endif
      eeprom_write(brokerChar, BROKER_IP_START);
      setup_mqtt();
    } // if (server.hasArg("broker"))
    server.send(200, "text/html", htmlBodyThx);
  } // if ( server.hasArg("broker_data") )
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
  WiFi.softAPmacAddress(mac);
  macID =  String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
           String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  DeviceNameString = "NODE_" + macID;
//  char DeviceNameChar[DeviceNameString.length() + 1];
  DeviceNameString.toCharArray(DeviceNameChar, DeviceNameString.length() + 1);
  Serial.print("Connect to network: ");
  Serial.println(DeviceNameChar);

  WiFi.softAP(DeviceNameChar, ACCESS_POINT_PASS);

  IPAddress apip = WiFi.softAPIP();
#if DEBUG
  Serial.print("Visit: ");
  Serial.println(apip);
#endif
  server.on("/", handle_root);
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
void eeprom_clear(uint8_t from, uint8_t to)
{
#if DEBUG
  Serial.println(F("Clearing EEPROM."));
#endif
  for (uint8_t i = from ; i <= to; i++)
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
  WiFi.mode(WIFI_AP_STA);
  WiFi.onEvent(wifi_event); // setup event function to handle wifi events
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
  digitalWrite(GREEN_LED, HIGH);
}
void led_wifi_off(void)
{
  digitalWrite(GREEN_LED, LOW);
}
void led_ap_on(void)
{
  digitalWrite(YELLOW_LED, HIGH);
}
void led_ap_off(void)
{
  digitalWrite(YELLOW_LED, LOW);
}
