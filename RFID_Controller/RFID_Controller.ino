/*
Code summary: This code controller a MFRC522 RFID card reader using an ESP8266 over the SPI Bus. The ESP8266 connects to Wi-Fi then continuously waits for a RFID to be read by the sensor.
              Once a card is detected its ID is read to memory and sent over Wi-Fi to a Firebase database. If the ESP8266 ever loses Wi-Fi it will attempt to reconnect 
              to its last known connection.
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SPI.h>
#include <MFRC522.h>

//RFID SPI Bus pins
#define SS_PIN D2
#define RST_PIN D1
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the RFID class 


// Internet router credentials
#define WIFI_SSID "XT1565"
#define WIFI_PASSWORD "Bing12345"

//Firebase database credentials
#define FIREBASE_HOST "cpe556-weatherstation.firebaseio.com"
#define FIREBASE_AUTH "dj9GOz0km2O9IbWZDsrntdksLsJ0HSLaAgK0RSEQ"


String RFIDcheck(){
  // Look for new cards
  String statuss = "";
   if ( ! mfrc522.PICC_IsNewCardPresent()) //Scan for any card infront of sensor
  {
    return statuss;
  }
  // Verify if the NUID has been readed
  if ( ! mfrc522.PICC_ReadCardSerial()) //Read data from card that was scanned
   {
    return statuss;
   }
  Serial.println();
  Serial.print("UID tag: ");
  String content;
  byte readCard[4];
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    readCard[i] = mfrc522.uid.uidByte[i];   // Reads RFID cards UID.
    Serial.print(readCard[i], HEX);         // Prints RFID cards UID to the serial monitor.
    content.concat(String(readCard[i], HEX));
  }
  content.toUpperCase();
  statuss=content;
  Serial.println();
  return statuss;
}

void setup()
{
  Serial.begin(9600);
  
  // connect to wifi.
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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Init Firebase connection
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 
}

void loop() 
{
 String RFIDStation =RFIDcheck(); //Read for any RFID cards

 //If RFID is read send its ID string to Firebase
 if (RFIDStation != ""){
  Firebase.setString("RFID Station", RFIDStation);
  if(Firebase.success()){
    Serial.println("Updated RFID");
    }
    else{
      Serial.println("Update failed");
    }
    Serial.println("");
 }
 delay(500);
}
