# CPE-556-WeatherStation-Project

This project is for the class CPE 556 Computing Principles of Mobile & Embedded Sys. at Stevens Institute of Technology. The project uses three ESP8266 NodeMCU development boards to implement an RFID tag system where one board is the controller and the two are slaves that gather sensor data. All the boards are connected to a Google hosted Firebase database that stores the data gathered and is read from for which RFID tage was scanned. This project also includes a MIT App Inventor 2 project that is a mobile app that connects to the Firebase database to pull data and display it on the smartphone screen. The ESP8266s were programmed using the Arduino IDE and are denoted by the .ino files.
 
## File Breakdown
 
### RFID_Controller.ino: 
Controlles a MFRC522 RFID card reader using an ESP8266 over the SPI Bus. The ESP8266 connects to Wi-Fi then continuously waits for a RFID to be read by the sensor. Once a card is detected its ID is read to memory and sent over Wi-Fi to a Firebase database. If the ESP8266 ever loses Wi-Fi it will attempt to reconnect to its last known connection.
 
### Station_1.ino & Station_2.ino:
This code controls a ESP8266 and two sensor boards to send temperaure, humidity, barometric and pressure to a Firebase database over Wi-Fiif a specific RFID card is read by the RFID controller. If the ESP8266 ever loses Wi-Fi it will attempt to reconnect to its last known connection.
 
### CPE556.apk:
This is the mobile app that on initlaize connects to the Firebase database and deletes all current values. Then every second the app pulls data from the database and displats it on the app in six different text fields along with the corresponding units of that measurement. If any error occurs with the connection to the Firebase an error message is displayed on the screen.