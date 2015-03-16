/*
  DHCP-based
 
 This sketch uses the DHCP extensions to the Ethernet library
 to get an IP address via DHCP and print the address obtained.
 using an Arduino Wiznet Ethernet shield.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
                             and pin 4 dor sd card
                             
 Sensore di temperatura LM35 su pin A3
 Sensore NC su    pin ?
 Rele cancello    pin
 Relé cancelletti pin
 
 created 12 Marzo 2015
 modified X Apr 2015
 by Tom Rugg
 doc:
 http://ismanettoneblog.altervista.org/blog/lezione-13-arduino-si-connette-ad-internet-shield-ethernet-ufficiale/
 */
 
#include <SPI.h>
#include <Ethernet.h>
#include "string.h"
#include "iotconnector.h"

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
 
char pubkey[] = "pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695";char subkey[] = "sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe";char channel[] = "cancello_client_arduino";char uuid[] = "arduino color-3";
 
iotbridge arduino;

//Variabili
unsigned long currentMillis = 0L;
unsigned long previousMillisTemp = 0L;
unsigned long premutoPulsanteCancelloMillis = 0L;

//rele
#define PIN_RELE_CANCELLO    2
#define PIN_RELE_CANCELLETTI 3

/* contatto magnetico normalmente chiuso con resistenza di pull-up interna
          contatto    NC
                  .--o---o--o GND
                  |    /\
 Pin 8 o-----------
 */
#define NC_CONT_1  8

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
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
   }
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
  // setto i contatti NC 
  pinMode(NC_CONT_1, INPUT);           // set pin to input
  digitalWrite(NC_CONT_1, HIGH);       // turn on pullup resistors
  
  // initialize the digital pin's as an output.
  pinMode(PIN_RELE_CANCELLO, OUTPUT);            // Relay 1 
  digitalWrite(PIN_RELE_CANCELLO, LOW);
  pinMode(PIN_RELE_CANCELLETTI, OUTPUT);         // Relay 2
  digitalWrite(PIN_RELE_CANCELLETTI, LOW);
  
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
  arduino.send(channel,"{\"avatar\": \"arduino color-2\", \"text\": \"Aperto cancello\"}");
   
  //Subscribe
  returnmessage = arduino.connect(channel);
  //callback function of sorts, to work with the received message
  do_something(returnmessage);
  Serial.println();
  apriCancello();
  readTemp();
  apriCancelletto();
  readSensori();
}

void readSensori(){
}

void apriCancello(){
  if((premutoPulsanteCancelloMillis + 2000 > currentMillis) && (currentMillis > premutoPulsanteCancelloMillis)){
    digitalWrite(PIN_RELE_CANCELLO, HIGH);
  } else {
    premutoPulsanteCancelloMillis = 0L;
  }
}

void apriCancelletto(){
  
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
