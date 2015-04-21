/* 
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield). 
  See the README file for more details.
 
 Sensore di temperatura LM35 su pin A1,A3
 Sensore NC su    pin 8
 Rele cancello    pin 2
 Relé cancelletti pin 3
 
 created 22 Aprile 2015
 modified X MAggio 2015
 by Tom Rugg 
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include "aREST.h"
#include <avr/wdt.h>

#define PIN_RELE_CANCELLO    2
#define PIN_RELE_CANCELLETTI 3

/* contatto magnetico normalmente chiuso con resistenza di pull-up interna 
porta di ingresso
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
// Variables to be exposed to the API
int LM35sensor = 0;
int LM35sensor_su_scheda = 0;
  
//Variabili
unsigned long currentMillis = 0L;
unsigned long previousMillisTemp = 0L;
unsigned long premutoPulsanteCancelloMillis = 0L;
  
// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// IP address in case DHCP fails
IPAddress ip(192,168,1,200);

// Ethernet server
EthernetServer server(80);

//variabili per pushingbox.com accesso con account gmail
char serverName[] = "api.pushingbox.com";
char DEVID1[] = "vD6CBA2419476BA2";

// Create aREST instance
aREST rest = aREST();


void setup(void)
{  
  // Start Serial
  Serial.begin(115200);
  
  // setto i contatti NC 
  pinMode(NC_CONT_1, INPUT);           // set pin to input porta di ingresso
  digitalWrite(NC_CONT_1, HIGH);       // turn on pullup resistors
  
  // initialize the digital pin's as an output.
  pinMode(PIN_RELE_CANCELLO, OUTPUT);            // Relay 1 
  digitalWrite(PIN_RELE_CANCELLO, LOW);
  pinMode(PIN_RELE_CANCELLETTI, OUTPUT);         // Relay 2
  digitalWrite(PIN_RELE_CANCELLETTI, LOW);
  
  // Init variables and expose them to REST API
  rest.variable("LM35sensor",&LM35sensor);
  rest.variable("LM35sensor_su_scheda",&LM35sensor_su_scheda);

  // Function to be exposed
  rest.function("portoncino",portoncinoControl);
  rest.function("cancello",cancelloControl);
  
  // Give name and ID to device
  rest.set_id("001");
  rest.set_name("ingresso");
  
  //setto variabili temeperatura
  LM35sensor = analogRead(tempPin);
  LM35sensor_su_scheda = analogRead(tempPin_su_scheda);
  

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
  wdt_enable(WDTO_8S);
}

void loop() {  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
  readIngresso();
  readTemp();
  wdt_reset();
}

// Custom function accessible by the API
int portoncinoControl(String command) {
  Serial.print("portoncinoControl: ");
  Serial.println(command);
  // Get state from command
  //int state = command.toInt();
  
  digitalWrite(PIN_RELE_CANCELLETTI,HIGH);
  delay(1000);
  digitalWrite(PIN_RELE_CANCELLETTI,LOW);
  return 1;
}

int cancelloControl(String command){
  Serial.print("cancelloControl: ");
  Serial.println(command);
  digitalWrite(PIN_RELE_CANCELLO,HIGH);
  delay(3000);
  digitalWrite(PIN_RELE_CANCELLO,LOW);
  return 1;
}

void readIngresso(){
  int ingresso = digitalRead(NC_CONT_1);
  if(ingresso){
    delay(200);
    if(digitalRead(NC_CONT_1)){
      allarmeMailPushingbox("Apertura porta di ingresso:", "ALLARME");
    }
  }
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
    if (client_eth.connect(serverName, 80) && (tentativi < 3)) {
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
