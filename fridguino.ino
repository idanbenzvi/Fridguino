#include <ESP8266WiFi.h>
#include "AlertMe.h" // This includes all of the references to WiFiManager and ArduinoJSON that we need
#include <TroykaDHT.h>

AlertMe alert;
DHT dht(4, DHT21);

const uint8_t config_pin = 12;
const double criticalTemprature = -10;
 
String to_email = "PUTMAILHERE@gmail.com";  // The email address to send the message to
String to_sms = "5551234567@carriersmsportal.com"; // Look up your carrier's Email-to-SMS gateway here: https://martinfitzpatrick.name/list-of-email-to-sms-gateways/

String subject_line = "Fridge Temprature Alert";
String message;

double previousTemp ; 
double currTemp ;
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

struct tempratureReading {
  String errorMsg;
  double temp;
}

tempratureReading measureTemprature() {
  dht.read();
  switch(dht.getState()) {    
    case DHT_OK:
      Serial.print("Temperature = ");
      Serial.print(dht.getTemperatureC());
      Serial.println(" C \t");
      Serial.print("Humidity = ");
      Serial.print(dht.getHumidity());
      Serial.println(" %");
      return new tempratureReading(null,dht.getTemperatureC());
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      return new tempratureReading("Error: Checksum error, sensor is broken",999);
    case DHT_ERROR_TIMEOUT:
      return new tempratureReading("Error: timeout error, sensor is broken",999);
    case DHT_ERROR_NO_REPLY:
      return new tempratureReading("Error: no response error, sensor is broken",999);
  }
}

boolean sendNotificaiton() {
  //TODO send email once the temprature is above a certain threshold
 
}
     
void loop() {
    while(true) {
    delay(60 * 15 * 1000)
    //todo add retry mechanism
    double currTemprature = measureTemprature()
    //todo add derivative algo. 


    
    message = "Device  "+ String(ESP.getChipId())+" : Temprature warning! the current temprature is " + currTemprature ;

    if(currTemprature > criticalTemprature) {
      Serial.print("Sending Email...");
      alert.send(subject_line,message,to_email))
    }
    
    

}
