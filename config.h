// uC type. EEPROM commit functionality (and others) depends on type of processor.
#define ATMEGA 1
#define ESP8266 0

#define MICROCONTROLLER ESP8266 // Set the used uC here
#define DEBUG 1 // The debug print outs are allowed here

#define SSID_ADDR_START 0 // The SSID starts on this EEPROM address
#define SSID_ADDR_STOP 31 // The SSID stops on this EEPROM address
#define PASS_ADDR_START 32 // The PASS starts on this EEPROM address
#define PASS_ADDR_STOP 63 // The PASS stops on this EEPROM address
#define BROKER_IP_START 64 // The broker IP starts on this EEPROM address
#define BROKER_IP_STOP 79 // The broker IP stops on this EEPROM address

#define END_OF_EEPROM_READ 0

#define WIFI_TIMEOUT 20 // 20 = 10s. Interval in which the device should connect to wifi network.

#define RED_LED 16
#define YELLOW_LED 5
#define GREEN_LED 4

#define ACCESS_POINT_NAME "IoT_node"
#define ACCESS_POINT_PASS "password"

#define MQTT_BROKER_PORT 1883

ESP8266WebServer server(80); // create a server object fot AP mode.

uint8_t mac[WL_MAC_ADDR_LENGTH];
String macID;
String DeviceNameString;
char DeviceNameChar[12];

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 200;

char ssid[32] = "";
char pass[32] = "";

int wifiTimeoutConn = 0; // The timeout for wifi connection.
int wifiTimeoutEvent = 0; // The timeout for wifi connection inside wifi event: conn lost.

char* htmlBody =
  "<!DOCTYPE HTML>"
  "<html>"
  "<head>"
  "<title>THE_NODE</title>"
  "</head>"
  "<body>"
  "<h1>Welcome to THE_NODE</h1>"
  "<h3>Provide the node with your wifi credentials:<br>"
  "<form action=\"/\" method=\"post\">"
  "<p>SSID</p>"
  "<input type=\"text\" name=\"ssid\"><br>"
  "<p>PASS</p>"
  "<input type=\"password\" name=\"pass\"><br>"
  "<input type=\"submit\" value=\"Connect\" name=\"wifi_data\">"
  "</form>"
  "<form action=\"/\" method=\"post\">"
  "<p>MQTT broker IP:</p>"
  "<input type=\"text\" name=\"broker\"><br>"
  "<input type=\"submit\" value=\"Setup broker\" name=\"broker_data\">"
  "</form>"
  "</body>"
  "</html>";

char* htmlBodyThx =
  "<!DOCTYPE HTML>"
  "<html>"
  "<head>"
  "<title>THE_NODE</title>"
  "</head>"
  "<body>"
  "<h1>Welcome to THE_NODE</h1>"
  //"<h3>Thank you, I'll try to connect to your wifi using <?php $_POST[\"ssid\"]; ?> and <?php $_POST[\"pass\"]; ?>."
  "<h3>Thank you, I'll try to connect to your wifi network."
  "</body>"
  "</html>";

char brokerChar[16] = ""; // Array for when reading from web and writing to EEPROM
char broker[16] = ""; // Array for when reading from EEPROM
WiFiClient espClient;
PubSubClient mqttClient(espClient);
