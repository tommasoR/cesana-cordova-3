/*
PubNub sample client
This sample client will use just the minimal-footprint raw PubNub
interface where it is your responsibility to deal with the JSON encoding.
It will just send a hello world message and retrieve one back, reporting
its deeds on serial console.
DHCP-based
 
 This sketch uses the DHCP extensions to the Ethernet library
 to get an IP address via DHCP and print the address obtained.
 using an Arduino Wiznet Ethernet shield.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
                             and pin 4 dor sd card
                             
 Sensore di temperatura LM35 su pin A3
 Sensore NC su    pin 8
 Rele cancello    pin 2
 Relé cancelletti pin 3
 
 created 22 Marzo 2015
 modified X Apr 2015
 by Tom Rugg
https://github.com/pubnub/pubnub-api/tree/master/arduino
This code is in the public domain.
*/
#include <SPI.h>
#include <Ethernet.h>
#include "PubNub.h"
// Some Ethernet shields have a MAC address printed on a sticker on the shield;
// fill in that address here, or choose your own at random:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEE, 0xEA };
//const int subLedPin = 8;
//const int pubLedPin = 9;
char pubkey[] = "pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695";
char subkey[] = "sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe";
char channel[] = "cancello_client_arduino";
char uuid[] = "xxxxxxxx-xxxx-3333-9999-xxxxxxxxxxxx";

void random_uuid() {
  randomSeed(analogRead(4) + millis() * 1024);
  snprintf(uuid, sizeof(uuid), "%04lx%04lx-%04lx-3333-9999-%04lx%04lx%04lx",
  random(0x10000), random(0x10000), random(0x10000),
  random(0x10000), random(0x10000), random(0x10000));
}
void setup(){
  Serial.begin(9600);
  Serial.println("Serial set up");
  while (!Ethernet.begin(mac)) {
    Serial.println("Ethernet setup error");
    delay(1000);
  }
  Serial.println("Ethernet set up");
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  PubNub.begin(pubkey, subkey);
  random_uuid();
  PubNub.set_uuid(uuid);
  Serial.println("PubNub set up");
}
void flash(int ledPin){
  /* Flash LED three times. */
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
void loop(){
  Ethernet.maintain();
  PubSubClient *client;
  Serial.println("waiting for a message (subscribe)");
  client = PubNub.subscribe(channel);
  if (!client) {
    Serial.println("subscription error");
    delay(1000);
    return;
  }
  Serial.print("Received: ");
    while (client->wait_for_data()) {
    char c = client->read();
    Serial.print(c);
  }
  client->stop();
  Serial.println();
  //flash(subLedPin);
  delay(200);
}
