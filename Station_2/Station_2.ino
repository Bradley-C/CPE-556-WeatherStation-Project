/*
Code summary: This code controls a ESP8266 and two sensor boards to send temperaure, humidity, barometric and pressure to a Firebase database over Wi-Fi
              if a specific RFID card is read by the RFID controller. If the ESP8266 ever loses Wi-Fi it will attempt to reconnect to its last known connection. 
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPL115A2.h>
#include "DHT.h"

//Internet router credentials
#define WIFI_SSID "XT1565"
#define WIFI_PASSWORD "Bing12345"

//Firebase database credentials
#define FIREBASE_HOST "cpe556-weatherstation.firebaseio.com"
#define FIREBASE_AUTH "dj9GOz0km2O9IbWZDsrntdksLsJ0HSLaAgK0RSEQ"

//DHT sensor pin connection and setup
#define DHTPIN D2     
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

//MPL115A2 senor pin connection and setup
#define MPL_SCL D5
#define MPL_SDA D6
Adafruit_MPL115A2 mpl115a2;


void setup(void) 
{
  Serial.begin(9600);
  
  //Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setAutoConnect(true); //Enable Wi-Fi autoconnect
  Serial.print("connecting");

  //Check for stable Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  dht.begin(); //Init DHT11

  //Init MPL15A2
  Wire.begin(MPL_SDA, MPL_SCL);
  mpl115a2.begin();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Init Firebase connection
}

void loop(void) 
{
  float pressureKPA = 0;
  float temperatureC = 0;
  float humidity = 0;
  
  //Read Firebase for the RFID of this station, if so begin reading sensor data and sending it to Firebase
  if (Firebase.getString("RFID Station")== "ABCAFAA"){

    //Peform temperaure, humidity, and barometric pressure sensor readings
    pressureKPA = mpl115a2.getPressure(); 
    temperatureC = mpl115a2.getTemperature();
    humidity = dht.readHumidity();
    
    Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.println(" kPa");
    Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C");
    Serial.print("Humidity % = ");
    Serial.println(humidity);

    //Set all sensor tags to current readings from sensors
    Firebase.setFloat("Temperature",temperatureC);
    if(Firebase.failed()){
      Serial.print(Firebase.error());
      }
    Firebase.setFloat("Humidity",humidity);
    if(Firebase.failed()){
      Serial.print(Firebase.error());
      }
    Firebase.setFloat("Pressure",pressureKPA);
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
    Firebase.setString("Gas","---");
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
    Firebase.setString("Altitude","---");
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
    Firebase.setString("UV Index","---");
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
  }
  delay(1000); // miliseconds before next reading
}
