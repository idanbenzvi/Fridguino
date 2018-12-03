#include <ESP8266WiFi.h>
#include "AlertMe.h" // This includes all of the references to WiFiManager and ArduinoJSON that we need
#include <TroykaDHT.h>

AlertMe alert;
DHT dht(4, DHT21);

const uint8_t config_pin = 12;
const double CRITCAL_TEMP = 10;
const double CRITCAL_DELTA = 3;
const int DHT_ERROR = 999;
 
String to_email = "etay1283@gmail.com";  // The email address to send the message to
String to_sms = "5551234567@carriersmsportal.com"; // Look up your carrier's Email-to-SMS gateway here: https://martinfitzpatrick.name/list-of-email-to-sms-gateways/

String subject_line = "Fridge Temprature Alert";
String message;

double previousTemp; 
/**
 * Fridguino by Idan Ben Zvi and Itay Ceder (c) 12/2018
 * The sketch sets up a wifi access point initially to get mail and wifi credentials.
 * After setup the device will constantly poll for the temprature once every 15 minutes.
 * If the temprature rises above a specified threshold a message will be sent.
 * TODO:
 * add derivative calculations and set a bar for alerts
 * add auto reconnect ability 
 * add SIM Gprs capability
 */
    void setup() {
      dht.begin();
      Serial.begin(115200); // the speed in which the serial USB connection is set
      delay(200);
     
      // We start by connecting to a WiFi network
     
  pinMode(config_pin,INPUT_PULLUP);
  if(config_pin == LOW){ // Short pin to GND for config AP
    alert.config();
  }
  
  alert.debug(true); // uncomment for debug/SMTP data
  
  Serial.print("\nConnecting to WIFI/SMTP...");
  alert.connect(); // Connect to WiFi, then test connect to SMTP server.
                   // If we fail with either, or they aren't configured,
                   // host an AP portal for configuration.
                   // alert.connect(true) prints WiFi debug info

  Serial.println("Connected!");
    }

double measureTemprature() {
  dht.read();
  switch(dht.getState()) {    
    case DHT_OK:
      Serial.print("Temperature = ");
      Serial.print(dht.getTemperatureC());
      Serial.println(" C \t");
      Serial.print("Humidity = ");
      Serial.print(dht.getHumidity());
      Serial.println(" %");
      return dht.getTemperatureC();
    case DHT_ERROR_CHECKSUM:
      return DHT_ERROR;
    case DHT_ERROR_TIMEOUT:
      return DHT_ERROR;
    case DHT_ERROR_NO_REPLY:
      return DHT_ERROR;
  }
}

void sendNotificaiton(String message) {
  Serial.print("Sending Email: " + message);
  alert.send(subject_line,message,to_email);
}
     
void loop() {
    while(true) {
    delay(60 * 15 * 1000);
    int retryAttemps = 3;
    double currTemprature;
    
    do {
      currTemprature = measureTemprature();
      retryAttemps--;    
    } while (currTemprature != DHT_ERROR && retryAttemps > 0);
    
    double currentDeltaTemp = currTemprature - previousTemp;
    
    if (currTemprature == DHT_ERROR) {
      message = "Device  " + String(ESP.getChipId()) + " Error: no temprature response return, sensor is broken";
      sendNotificaiton(message);
    } else if (currentDeltaTemp > CRITCAL_DELTA){
      message = "Device  " + String(ESP.getChipId()) + " : Delta Temprature is above trashold! the current temprature is " + currTemprature;
      sendNotificaiton(message);
    } else if(currTemprature > CRITCAL_TEMP) {
      message = "Device  " + String(ESP.getChipId()) + " : Temprature is above trashold! the current temprature is " + currTemprature;
      sendNotificaiton(message);
    }
    
    if (currTemprature != DHT_ERROR) {
      previousTemp = currTemprature;
    }
  }
}
