// uC type. EEPROM commit functionality (and others) depends on type of processor.
#define ATMEGA 1
#define ESP8266 0

#define MICROCONTROLLER ESP8266 // Set the used uC here
#define DEBUG 1 // The debug print outs are allowed here

#define SSID_ADDR_START 0 // The SSID starts on this EEPROM address
#define PASS_ADDR_START 32 // The PASS starts on this EEPROM address
#define END_OF_EEPROM_READ 0

#define SSID_PASS_SIZE strlen(pass_write) + strlen(ssid_write) + 1

#define WIFI_TIMEOUT 20 // 20 = 10s. Interval in which the device should connect to wifi network.

#define RED_LED 16
#define YELLOW_LED 5
#define GREEN_LED 4

#define ACCESS_POINT_NAME "IoT_node"
#define ACCESS_POINT_PASS "password"

ESP8266WebServer server(80); // create a server object fot AP mode.

char * ssid;
char * pass;
char buff[32] = ""; // buffer to store values read from eeprom

int wifiTimeout = 0; // The timeout for wifi connection.

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
"<input type=\"submit\" value=\"Connect\">"
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
