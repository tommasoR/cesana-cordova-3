#include <SPI.h>
#include <Ethernet.h>
#include "string.h"
#include "iotconnector.h"
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
char pubkey[] = "pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695";char subkey[] = "sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe";char channel[] = "cancello_client_arduino";char uuid[] = "arduino color-3";
 
iotbridge arduino;

//Variabili
unsigned long currentMillis = 0L;
unsigned long previousMillisTemp = 0L;

/* Sensor test sketch
    for more information see http://www.ladyada.net/make/logshield/lighttemp.html
    */
//LM35 Pin Variables
#define tempPin 1 //the analog pin the LM35's Vout (sense) pin is connected to
    //the resolution is 10 mV / degree centigrade with a
  float temperatureC = 0.0; // the analog reading from the sensor
  int LM35sensor = 0;
  float volts = 0.0;
 
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
  //int comando = stringOne.indexOf('ApriCancello');
}
void setup()
{
  initialize();
  arduino.init( pubkey, subkey, uuid);
}
 
void loop()
{
  currentMillis = millis();
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


void readTemp() {
  LM35sensor += analogRead(tempPin);
  LM35sensor /=2;
  if(currentMillis - previousMillisTemp > 5000) {
    // save the last time control
    previousMillisTemp = currentMillis;
    calcolaTemp();
  }
}

void calcolaTemp(){
  //getting the voltage reading from the temperature sensor
    //LM35sensor = analogRead(tempPin);
    //lettura valore del sensore LM35 messo sull'ingresso
    //analogico A1
    volts = ( LM35sensor/1024.0)*5.0; //formula per ottenere la tensione di uscita dell'LM35 in volts
    temperatureC =volts*100.0;// valore espresso in gradi Celsius (l'out del sensore è 10mv per grado) 
    // print out the voltage
    Serial.print(volts); Serial.println(" volts");
    // now print out the temperature
    Serial.print(temperatureC); Serial.println(" degrees C");
}
/*
- See more at: file:///D:/googleCode/cesana-cordova-3/arduino/doc/Connect%20Arduino%20to%20PubNub%20in%202%20Steps%20-%20PubNub.htm#sthash.qsLr0u6g.dpuf
*/
