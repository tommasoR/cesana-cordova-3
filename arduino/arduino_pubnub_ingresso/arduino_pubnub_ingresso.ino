#include <SPI.h>
#include <Ethernet.h>
#include "string.h"
#include "iotconnector.h"
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
char pubkey[] = "PUBLISH KEYS HERE";char subkey[] = "SUBSCRIBE KEYS HERE";char channel[] = "iotchannel";char uuid[] = "Arduino";
 
iotbridge arduino;
 
void initialize(){
Serial.begin(9600);
Serial.println("Serial set up");
 
while (!Ethernet.begin(mac)) {
Serial.println("Ethernet setup error");
delay(1000);
}
Serial.println("Ethernet set up");
}
 
void do_something(String value){
Serial.println("in the callback");
Serial.println(value);
}
void setup()
{
initialize();
arduino.init( pubkey, subkey, uuid);
}
 
void loop()
{
String returnmessage;
Ethernet.maintain();
//Publish
arduino.send(channel,"\"Hey There\"");
 
//Subscribe
returnmessage = arduino.connect(channel);
//callback function of sorts, to work with the received message
do_something(returnmessage);
Serial.println();
}
// See more at: http://www.pubnub.com/blog/connect-arduino-to-pubnub-in-2-steps/#sthash.LT3iK3M2.dpuf
