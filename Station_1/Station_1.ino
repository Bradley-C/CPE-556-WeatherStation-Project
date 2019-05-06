/*
Code summary: This code controller uses a ESP8266 and two sensor boards to send temperaure, humidity, barometric pressure, altitude, UV index, and air quality to a Firebase database over Wi-Fi
              if a specific RFID card is read by the RFID controller. If the ESP8266 ever loses Wi-Fi it will attempt to reconnect to its last known connection. 
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "Adafruit_SI1145.h"

// Internet router credentials
#define WIFI_SSID "XT1565"
#define WIFI_PASSWORD "Bing12345"

//Firebase database credentials
#define FIREBASE_HOST "cpe556-weatherstation.firebaseio.com"
#define FIREBASE_AUTH "dj9GOz0km2O9IbWZDsrntdksLsJ0HSLaAgK0RSEQ"

//BME680 sensor connection pins and setup for SPI
#define BME_SCK D2
#define BME_MISO D0
#define BME_MOSI D1
#define BME_CS D5
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

// SI1145 sensor connection pins and setup for I2C
#define SI_SCL D3
#define SI_SDA D4
Adafruit_SI1145 uv = Adafruit_SI1145();

//*******PRINT OUT READINGS TO SERIAL******
void BME680_sensor() {
   Serial.print("Temperature = ");
   Serial.print(bme.temperature);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 1000.0);
  Serial.println(" kPa");
  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");
  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
}

//******SETUP************
void setup() {
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

    //Init BME680
    if (!bme.begin()) {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
      while (1);
      }
      
    //Init SI1145 and SPI ports
    Wire.begin(SI_SDA, SI_SCL);  
    if (!uv.begin()) {
      Serial.println("Could not find a valid Si1145 sensor, check wiring!");
      while (1);
      }
      
    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Init Firebase connection 
}

void loop() {

  //Read Firebase for the RFID of this station, if so begin reading sensor data and sending it to Firebase
  if (Firebase.getString("RFID Station")== "AA495BD3"){

  //Peform temperaure, humidity, barometric pressure, altitude, and air quality sensor readings to bme class
  bme.performReading();
  BME680_sensor();

  //Read and calculate UV index from sensor
  float UVindex = uv.readUV();
  UVindex /= 100.0;
  Serial.print("UV index = ");
  Serial.print(UVindex);
  Serial.println();

  //Set all sensor tags to current readings from sensors
    Firebase.setFloat("Temperature",bme.temperature);
    if(Firebase.failed()){
      Serial.print(Firebase.error());
      }
    Firebase.setFloat("Humidity",bme.humidity);
    if(Firebase.failed()){
      Serial.print(Firebase.error());
      }
    Firebase.setFloat("Pressure",bme.pressure / 1000.0);
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
    Firebase.setFloat("Gas",bme.gas_resistance / 1000.0);
    if(Firebase.failed()){
      Serial.print(Firebase.error());  
      }
    Firebase.setFloat("Altitude",abs(bme.readAltitude(SEALEVELPRESSURE_HPA)));
    if(Firebase.failed()){
       Serial.print(Firebase.error());  
       }
    Firebase.setFloat("UV Index",UVindex);
    if(Firebase.failed()){
       Serial.print(Firebase.error());  
       }
    }
 delay(1000); // miliseconds before next reading
}
