#include <EEPROM.h>

// uC type
#define ATMEGA 1
#define ESP8266 0

#define MICROCONTROLLER ATMEGA // Set the used uC here
#define WRITE_FUNCTIONALITY 1 // Set to 1 to include EEPROME write

#define SSID_ADDR_START 0
#define PASS_ADDR_START 50
#define END_OF_EEPROM_READ 0

int eeprom_address = SSID_ADDR_START;
int eeprom_read_ssid = !END_OF_EEPROM_READ;
int eeprom_read_pass = !END_OF_EEPROM_READ;

char ssid[50] = ""; // array to store the ssid
char pass[50] = ""; // array to store the password

char ssid_temp[] = "***";
char pass_temp[] = "***";

//void eeprom_write_ssid_pass(char ssid_write[], char pass_write);
//void eeprom_clear (void);

void setup() {
#if MICROCONTROLLER == ESP8266 // If the uC is ESP, there is need to call EEPROM.begin()
  EEPROM.begin(512);
#endif
  delay(500);
  Serial.begin(9600);
  delay(500);
//  eeprom_clear();
//  eeprom_write_ssid_pass(ssid_temp, pass_temp);




//========================================SSID READ========================================//
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
  Serial.print("SSID: [");
  Serial.print(ssid);
  Serial.println("]");
  //======================================END SSID READ======================================//
 //=========================================PASS READ=========================================//
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
 //=======================================END PASS READ=======================================//
}

void loop()
{

  // read a byte from the current address of the EEPROM

}

//******************************************************************************
// EEPROM WRITE
// Writes temporary data to EEPROM for testing.
// Later will be used to write data to EEPROM
//******************************************************************************
void eeprom_clear(void) {
  Serial.println("Clearing EEPROM.");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM cleared.");
}


//******************************************************************************
// eeprom_write_ssid_pass (ssid_write, pass_write)
// Writes temporary data to EEPROM for testing.
// Later will be used to write data to EEPROM
//******************************************************************************
void eeprom_write_ssid_pass(char * ssid_write, char * pass_write) {
  Serial.print("Writting SSID[");
  Serial.print(ssid_write);
  Serial.print("] to EEPROM with size of ");
  Serial.println(strlen(ssid_write));
  for (int i = 0; i < strlen(ssid_write); i++) {
    Serial.print("Writing ");
    Serial.println(ssid_write[i]);
    EEPROM.write(eeprom_address, ssid_write[i]);
    eeprom_address = eeprom_address + 1;
    delay(100);
  }
  Serial.println("SSID writing done.");

  Serial.print("Writting PASS[");
  Serial.print(pass_write);
  Serial.print("] to EEPROM with size of ");
  Serial.println(strlen(pass_write));
  for (int i = 0; i < strlen(pass_write); i++) {
    Serial.print("Writing ");
    Serial.println(pass_write[i]);
    EEPROM.write(eeprom_address, pass_write[i]);
    eeprom_address = eeprom_address + 1;
    if ( strlen(pass_write) == eeprom_address ) {
      eeprom_address = 0; // reset the EEPROM address
      #if MICROCONTROLLER == ESP8266
      EEPROM.commit(); // Commit changes
      #endif
    }
    delay(100);
  }
  Serial.println("PASS writing done.");
}
