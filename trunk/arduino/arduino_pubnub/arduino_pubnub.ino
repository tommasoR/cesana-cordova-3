#include <SPI.h>
#include <Ethernet.h>
#include "string.h"
#include "iotconnector.h"
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
char pubkey[] = "pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695";char subkey[] = "sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe";char channel[] = "cancello_client_arduino";char uuid[] = "arduino color-3";
 
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
//int avatarEsist = stringOne.indexOf('avatar');
//int comando = stringOne.indexOf('apri');
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
//arduino.send(channel,"\"Hey There\"");
//esempio messaggio {"avatar": "arduino color-3", "text": "aperto"}
arduino.send(channel,"{\"avatar\": \"arduino color-3\", \"text\": \"aperto\"}");
 
//Subscribe
returnmessage = arduino.connect(channel);
//callback function of sorts, to work with the received message
do_something(returnmessage);
Serial.println();
}
/*
- See more at: file:///D:/googleCode/cesana-cordova-3/arduino/doc/Connect%20Arduino%20to%20PubNub%20in%202%20Steps%20-%20PubNub.htm#sthash.qsLr0u6g.dpuf
*/
