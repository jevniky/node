#include <EEPROM.h>

#define DEBUG 1

// uC type
#define ATMEGA 1
#define ESP8266 0

#define MICROCONTROLLER ESP8266 // Set the used uC here
#define WRITE_FUNCTIONALITY 1 // Set to 1 to include EEPROME write

#define SSID_ADDR_START 0
#define PASS_ADDR_START 50
#define END_OF_EEPROM_READ 0

int eeprom_address = SSID_ADDR_START;
int eeprom_read_ssid = !END_OF_EEPROM_READ;
int eeprom_read_pass = !END_OF_EEPROM_READ;

char ssid_temp[] = "UPC1234567";
char pass_temp[] = "ABCDEFGH";

char ssid[50] = ""; // array to store the ssid
char pass[50] = ""; // array to store the password

void setup() {
#if MICROCONTROLLER == ESP8266 // If the uC is ESP, there is need to call EEPROM.begin()
  EEPROM.begin(512);
#endif
  delay(500);
#if DEBUG
  Serial.begin(9600);
  delay(500);
#endif
  //====================================WRITE FUNCTIONALITY====================================//
#if WRITE_FUNCTIONALITY
  for (int i = 0; i < sizeof(ssid_temp); i++) {
    EEPROM.write(eeprom_address, ssid_temp[i]);
    eeprom_address = eeprom_address + 1;
    if ( sizeof(ssid_temp) == eeprom_address ) {
      //eeprom_address = 0;
#if MICROCONTROLLER == ESP8266 // If the uC is ESP, there is need to call EEPROM.commit()
      EEPROM.commit();
#endif
    }
    delay(100);
  }
  for (int i = 0; i < sizeof(pass_temp); i++) {
    EEPROM.write(eeprom_address, pass_temp[i]);
    eeprom_address = eeprom_address + 1;
    if ( sizeof(pass_temp) == eeprom_address ) {
      eeprom_address = 0;
#if MICROCONTROLLER == ESP8266 // If the uC is ESP, there is need to call EEPROM.commit()
      EEPROM.commit();
#endif
    }
    delay(100);
  }
#endif
  //==================================END WRITE FUNCTIONALITY==================================//
  //==================================SSID READ FUNCTIONALITY==================================//
  int i = 0; // array index counter
  eeprom_address = SSID_ADDR_START; // reset address for reading
  while ( END_OF_EEPROM_READ != eeprom_read_ssid ) {
    eeprom_read_ssid = EEPROM.read(eeprom_address);
    if ( '\0' != char(eeprom_read_ssid) ) {
      ssid[i] = char(eeprom_read_ssid);
      i++;
    }
    eeprom_address = eeprom_address + 1; // advance to the next address of the EEPROM
    delay(100);
  }
  #if DEBUG
  Serial.print("SSID: [");
  Serial.print(ssid);
  Serial.println("]");
  #endif
  //================================END SSID READ FUNCTIONALITY================================//
 //==================================PASS READ FUNCTIONALITY==================================//
 i = 0; // array index counter reset
 while ( END_OF_EEPROM_READ != eeprom_read_pass ) {
   eeprom_read_pass = EEPROM.read(eeprom_address);
   if ( '\0' != char(eeprom_read_pass) ) {
     pass[i] = char(eeprom_read_pass);
     i++;
   }
   eeprom_address = eeprom_address + 1; // advance to the next address of the EEPROM
   delay(100);
 }
 #if DEBUG
 Serial.print("PASS: [");
 Serial.print(pass);
 Serial.println("]");
 #endif
 eeprom_address = SSID_ADDR_START; // Set back the address to the start
 //================================END PASS READ FUNCTIONALITY================================//
}

void loop()
{

  // read a byte from the current address of the EEPROM

}
