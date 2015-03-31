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
                             
 Sensore di temperatura LM35 su pin A1,A3
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
/* DNS
Per configurare il dns senza modificare il metodo dhcp bisogna modificare nella classe Ethernet.cpp all'interno del metodo
int EthernetClass::begin(uint8_t *mac_address) la linea 
_dnsServerAddress = _dhcp->getDnsServerIp(); 
con
_dnsServerAddress = IPAddress(8,8,8,8).raw_address();

*/
char pubkey[] = "pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695";
char subkey[] = "sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe";
char channel[] = "cancello_client_arduino";
char uuid[] = "xxxxxxxx-xxxx-3333-9999-xxxxxxxxxxxx";

//variabili per pushingbox.com accesso con account gmail
char serverName[] = "api.pushingbox.com";
char DEVID1[] = "vD6CBA2419476BA2";
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
//EthernetClient client_eth;
// fine 

//Variabili
unsigned long currentMillis = 0L;
unsigned long previousMillisTemp = 0L;
unsigned long premutoPulsanteCancelloMillis = 0L;
//char messaggio_ricevuto[100];

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
#define tempPin A2
#define tempPin_su_scheda A2 //the analog pin the LM35's Vout (sense) pin is connected to
    //the resolution is 10 mV / degree centigrade with a
  float temperatureC = 0.0; // the analog reading from the sensor
  float temperatureC_su_scheda = 0.0;
  int LM35sensor = 0;
  int LM35sensor_su_scheda = 0;
 
void random_uuid() {
  randomSeed(analogRead(4) + millis() * 1024);
  snprintf(uuid, sizeof(uuid), "%04lx%04lx-%04lx-3333-9999-%04lx%04lx%04lx",
  random(0x10000), random(0x10000), random(0x10000),
  random(0x10000), random(0x10000), random(0x10000));
}

void initialize(){
  //Serial.begin(9600);
  //Serial.println("Serial set up");
  while (!Ethernet.begin(mac)) {
    //Serial.println("Ethernet setup error");
    delay(1000);
  }
  
  //Serial.println("Ethernet set up");
  //Serial.print("My IP address: ");
  //Serial.println(Ethernet.localIP());
  PubNub.begin(pubkey, subkey);
  random_uuid();
  PubNub.set_uuid(uuid);
  //Serial.println("PubNub set up");
}

void setup() {
  // setto i contatti NC 
  pinMode(NC_CONT_1, INPUT);           // set pin to input
  digitalWrite(NC_CONT_1, HIGH);       // turn on pullup resistors
  
  // initialize the digital pin's as an output.
  pinMode(PIN_RELE_CANCELLO, OUTPUT);            // Relay 1 
  digitalWrite(PIN_RELE_CANCELLO, LOW);
  pinMode(PIN_RELE_CANCELLETTI, OUTPUT);         // Relay 2
  digitalWrite(PIN_RELE_CANCELLETTI, LOW);
  
  initialize();
}

void loop(){
  currentMillis = millis();
  String returnmessage;
  Ethernet.maintain();
  inner_loop_sensori();
  PubSubClient *client;
  //Serial.println("waiting for a message (subscribe)");
  client = PubNub.subscribe(channel);
  if (!client) {
    //Serial.println("subscription error");
    delay(1000);
    return;
  } else {
    //Serial.print("Received: ");
   while (client->wait_for_data()) {
    char c = client->read();
    //Serial.print(c);
     if(c=='_'){
       c = client->read();
      if(c=='Z'||c=='Y'||c=='K'){
        eseguire(c);
      } 
     }
   }
   client->stop();
  }
  
  //Serial.println();
  delay(200);
}

/*void decodifica(String msg){
  Serial.println(messaggio_ricevuto);
  memset(&messaggio_ricevuto[0], 0, sizeof(messaggio_ricevuto));
}*/

void eseguire(char c ){
  if(c=='Z'){
    apriCancello();
  } else if(c=='Y'){
    apriCancelletto();
  }
  //invia sempre temperatura :-)
  //if(c=='K'){
  inviaTemperatura();
  //}
}

void inner_loop_sensori(){
  readTemp();
  readSensori();
}

void readSensori(){
}

void apriCancello(){
  if(premutoPulsanteCancelloMillis + 5000 < currentMillis) {
    digitalWrite(PIN_RELE_CANCELLO, HIGH);
    premutoPulsanteCancelloMillis=currentMillis;
    delay(500);
    digitalWrite(PIN_RELE_CANCELLO, LOW);
    EthernetClient *client_local;
    client_local = PubNub.publish(channel,"{\"avatar\": \"arduino color-2\", \"text\": \"Aperto cancello\"}");
    if ( !client_local) {
      delay(1000);
    } else{
      client_local->stop();
    }
  } else if(premutoPulsanteCancelloMillis > currentMillis){
    premutoPulsanteCancelloMillis=0L;
  }
}

void apriCancelletto(){
  
}

void inviaTemperatura(){
  EthernetClient *client_local;
  char buff[10];
  char str[80];
  dtostrf(temperatureC_su_scheda, 2, 2, buff);
  //char str[80]="{\"avatar\":\"Arduino_temp_scheda\", \"text\":\"22\"}";
  //client_local =PubNub.publish(channel,"{\"avatar\":\"Arduino_temp_scheda\", \"text\":\"22\"}");
  sprintf(str, "{\"avatar\":\"Arduino_temp_scheda\", \"text\":\"%s\"}", buff);
  client_local =PubNub.publish(channel,str);
  delay(1000);
  client_local->stop();
}

void readTemp() {
  LM35sensor += analogRead(tempPin);
  LM35sensor /=2;
  LM35sensor_su_scheda += analogRead(tempPin_su_scheda);
  LM35sensor_su_scheda /=2;
  if(currentMillis - previousMillisTemp > 5000) {//portare a un minuto
    // save the last time control
    previousMillisTemp = currentMillis;
    float volts = 0.0;
    //getting the voltage reading from the temperature sensor
    //LM35sensor = analogRead(tempPin);
    //lettura valore del sensore LM35 messo sull'ingresso
    //analogico A1
    volts = ( LM35sensor/1024.0)*5.0; //formula per ottenere la tensione di uscita dell'LM35 in volts
    temperatureC =volts*100.0;// valore espresso in gradi Celsius (l'out del sensore è 10mv per grado) 
    // print out the voltage
 //Serial.print(volts); Serial.println(" volts");
    // now print out the temperature
 //Serial.print(temperatureC); Serial.println(" sensore di temperatura esterno C");
    volts = ( LM35sensor_su_scheda/1024.0)*5.0;
    temperatureC_su_scheda=volts*100.0;
    // now print out the temperature
 //Serial.print(temperatureC_su_scheda); Serial.println(" sensore di temperatura su scheda C");
    
    if(temperatureC > 45){
      char buff[10];
      //dtostrf(FLOAT,WIDTH, PRECISION,BUFFER);
      dtostrf(temperatureC, 2, 2, buff);
      allarmeMailPushingbox("Temperatura in ingresso:", buff);
    }
    if(temperatureC_su_scheda > 50){
      char buff[10];
      dtostrf(temperatureC_su_scheda, 2, 2, buff);
      allarmeMailPushingbox("Temperatura su scheda arduino:", buff);
    }
  }
}


void allarmeMailPushingbox(String sensore, String messaggio){
  /*
  http://api.pushingbox.com/pushingbox?devid=vD6CBA2419476BA2&sensore='lm35'&messaggio='temperatura superiore 50 gradi'
  */
    EthernetClient client_eth;
    int tentativi=0;
 allarmeMail:   
    client_eth.stop();
    //Serial.println("connecting pushingbox...");
    if (client_eth.connect(serverName, 80) && (tentativi<10)) {
      //Serial.println("connected");
      //Serial.println("sendind request");
      client_eth.print("GET /pushingbox?devid=");
      client_eth.print(DEVID1);
      client_eth.print("&sensore='");
      client_eth.print(sensore);
      client_eth.print("'&messaggio='");
      client_eth.print(messaggio);
      client_eth.print("'");
      client_eth.println(" HTTP/1.1");
      client_eth.print("Host: ");
      client_eth.println(serverName);
      client_eth.println("User-Agent: Arduino");
      client_eth.println();
      tentativi++;
    } else {
      tentativi++;
      delay(1000);
      goto allarmeMail;
    }
    client_eth.stop();
}



