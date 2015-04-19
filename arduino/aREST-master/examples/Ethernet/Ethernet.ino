/* 
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield). 
  See the README file for more details.
 
  Written in 2014 by Marco Schwartz under a GPL license. 
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include "aREST.h"
#include <avr/wdt.h>

#define PIN_RELE_CANCELLO    2

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// IP address in case DHCP fails
IPAddress ip(192,168,1,100);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
int temperature;
int humidity;

void setup(void)
{  
  // Start Serial
  Serial.begin(115200);
  
  pinMode(PIN_RELE_CANCELLO, OUTPUT);            // Relay 1 
  digitalWrite(PIN_RELE_CANCELLO, LOW);
  
  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  // Function to be exposed
  rest.function("led",ledControl);
  rest.function("cancello",cancelloControl);
  
  // Give name and ID to device
  rest.set_id("008");
  rest.set_name("dapper_drake");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Start watchdog
  wdt_enable(WDTO_4S);
}

void loop() {  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
  
}

// Custom function accessible by the API
int ledControl(String command) {
  Serial.print("ledControl: ");
  Serial.println(command);
  // Get state from command
  //int state = command.toInt();
  
  digitalWrite(PIN_RELE_CANCELLO,HIGH);
  delay(1000);
  digitalWrite(PIN_RELE_CANCELLO,LOW);
  return 1;
}

int cancelloControl(String command){
  Serial.println(command);
  digitalWrite(PIN_RELE_CANCELLO,HIGH);
  delay(3000);
  digitalWrite(PIN_RELE_CANCELLO,LOW);
  return 1;
}
