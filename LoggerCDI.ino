#include <BluetoothSerial.h> //this library allows use of the Bluetooth serial features of the esp32
#include <SD.h>
#include "RTClib.h"

BluetoothSerial SerialBT; //rename the Bluetooth serial features to be more recognizable

#define cardSelect 33 

RTC_PCF8523 rtc; // setup real time clock name
File logfile;  //setup and placeholder for SD card file

void setup() {
  Serial.begin(115200); //start serial communication with the Arduino serial monitor
  Serial1.begin(115200); //start serial communication with the CDI port (defaults to the TX/RX pins)
  rtc.begin();
  
  //set up serial communication
  SerialBT.register_callback(callback); //call the callback function -- has to to do with the connection over Bluetooth
  if(!SerialBT.begin("ESP32")){
    Serial.println("An error occurred initializing Bluetooth"); //if the serial communication with Bluetooth doesn't begin, print this to Arduino monitor
  }
  else{
    Serial.println("Bluetooth initialized"); //if the serial communication with Bluetooth does begin, print this to the Arduino monitor
  }
  Serial.println("CDI SD logger test");
  
  // set up SD card communication
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
  }

  char filename[15];
  strcpy(filename, "/CDILOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i / 10;
    filename[8] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if ( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
  }
  Serial.print("Writing to ");
  Serial.println(filename);
  Serial.println("Ready!");
}

void loop (){
  while(1){
      DateTime now = rtc.now();
    if (Serial1.available()) {
        logfile.print(now.year(), DEC); logfile.print('/'); logfile.print(now.month(), DEC); logfile.print('/'); logfile.print(now.day(), DEC);logfile.print(" - ");logfile.print(now.hour(), DEC); logfile.print(':'); logfile.print(now.minute(), DEC); logfile.print(':'); logfile.println(now.second(), DEC);
        logfile.println(Serial1.readString());
        //SerialBT.write(Serial1.read()); //if there is data available over CDI, write it to the Bluetooth monitor
      }
    logfile.flush();
    DateTime now = rtc.now();
  }
  }

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){ //function to check if something is connected to the Bluetooth
  if(event == ESP_SPP_SRV_OPEN_EVT){
    SerialBT.println("Client Connected");
  }

  if(event == ESP_SPP_CLOSE_EVT ){
    SerialBT.println("Client disconnected");
  }
}