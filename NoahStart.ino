//Noah Z. 2019-11-30
//This program is a simple smash up of example files to prove simple button control,
//reading a battery voltage (soon to be CDI), having a counter and Real Time Clock,
//displaying it to a screen and also writing it to a uSD card.
//todo:
////play with external AND internal RealTimeClock, determine power consumption
//Make GUI and Buttons useful.
//Add serial input from CDI.
//Read up on power consumption and writing to SD. may be better to buffer 2 minutes
//and then write to SD. it may be power hungery to always be writing.
//Add RTC timestamp to file creation, windows shows 1980 for all files on SD.
//OLED is using I2C (and RTC?), SPI may be less power consumption.
//Look into Four-bit SD bus mode, and esp32 internal SD processing.
//Read up on esp32 low power modes
//Add bluetooth and/or wifi options

#include "RTClib.h"
#include <Adafruit_FeatherOLED.h>
#include <SPI.h>
#include <SD.h>

// Set the pins used
#define cardSelect 33  //CS pin on Featherwing OLED 128x32 pcb.
#define VBATPIN A13 //Featherwing ESP32 battery voltage divider analog pin. 
#define DEBUG_LED 13 //Featherwing ESP32 Green Debug LED on PCB
#define BUTTON_A 15 //Featherwing OLED buttion A pin is 15
#define BUTTON_B 32 //Featherwing OLED buttion B pin is 32
#define BUTTON_C 14 //Featherwing OLED buttion C pin is 14

RTC_PCF8523 rtc; // setup real time clock name
Adafruit_FeatherOLED oled = Adafruit_FeatherOLED(); // setup and use 'oled' for screen
File logfile;  //setup and placeholder for SD card file

float BAT_VOLT = 0; //float to save battery voltage to write to multiple places.
int count = 0; // integer variable to hold current counter value
int state = 0;  // integer variable to hold screen state; 0=0ff, 1== on

void setup() {
  Serial.begin(115200);
  Serial.println("CDI SD logger test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(DEBUG_LED, OUTPUT);  //Pin on featherwing esp32 that is LED.

  oled.init(); // start oled screen
  oled.setBatteryVisible(true); //allow battery icon on screen

  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);  //blink error pattern
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
    error(3);
  }
  Serial.print("Writing to ");
  Serial.println(filename);
  Serial.println("Ready!");
}


void loop() {
  // clear display
  oled.clearDisplay();
  oled.display();

  //dumb stuff to show the use of buttons, probably could be used better as interrupts
  if (!digitalRead(BUTTON_A)) state = 1;
  if (!digitalRead(BUTTON_B)) state = 0;
  if (!digitalRead(BUTTON_C)) oled.print("C");
  delay(10);

  //Future development needs to figure out how to take ONE measurement and save to file AND print to OLED.
  BAT_VOLT = getBatteryVoltage(); // get current battery voltage

  //get time from real time clock and save to now
  DateTime now = rtc.now();

  digitalWrite(DEBUG_LED, HIGH);//blip status LED on featherwing when writing

  //I forgot how to combine varibles and strings on one print statement below
  //below prints day and time to logfile- could probably be turned into a function
  logfile.print(now.year(), DEC); logfile.print('/'); logfile.print(now.month(), DEC); logfile.print('/'); logfile.print(now.day(), DEC);
  logfile.print(" - ");
  logfile.print(now.hour(), DEC); logfile.print(':'); logfile.print(now.minute(), DEC); logfile.print(':'); logfile.print(now.second(), DEC);
  logfile.println();
  logfile.print(count); logfile.print("- A13 = "); logfile.println(BAT_VOLT); // save to file
  logfile.println();
  
  logfile.flush();  //this needed to actually write and flush buffer but very power hungery, find another way?
  digitalWrite(DEBUG_LED, LOW);//blip led off on featherwing when writing

  //below prints day and time to serial- could probably be turned into a function
  Serial.print(now.year(), DEC); Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/'); Serial.print(now.day(), DEC);
  Serial.print(" - ");

  Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC); Serial.print(':'); Serial.print(now.second(), DEC);
  Serial.println();

  //below prints actual readings of seconds counter from last reset and voltage
  Serial.print(count); Serial.print(" seconds running- A13 = "); Serial.println(BAT_VOLT);  //print to debug serial
  Serial.println();

  // update the battery icon
  oled.setBattery(BAT_VOLT);
  oled.renderBattery();
  // print the count value to the OLED
  oled.print("count: ");
  oled.println(count);

  // This is where the screen is actually drawn- update the display with the new count
  if (state == 0) oled.display(); //this is where the buttons can turn on and off the screen


  count++;   // increment the counter by 1

  //it takes about 50ms to do above calculations so 1000-50=950ms
  delay(950);  // delay 1 second so we dont fill up the memory card with measurements.

}



//feathers esp32
float getBatteryVoltage() {
  float measuredvbat = analogRead(VBATPIN);

  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.8;  // Multiply by 3.3V, our reference voltage, 3.8 seems more accurate???
  measuredvbat /= 4096; // convert to voltage (max analog reading)

  return measuredvbat;
}


// blink out an error code
void error(uint8_t errno) {
  while (1) {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(DEBUG_LED, HIGH); delay(100);
      digitalWrite(DEBUG_LED, LOW); delay(100);
    }
    for (i = errno; i < 10; i++) {
      delay(200);
    }
  }
}







// //// Adafruit OLED FeatherWing Battery Example
// #include <Wire.h>
// //#include <Adafruit_GFX.h>
// //#include <Adafruit_SSD1306.h>
// #include <Adafruit_FeatherOLED.h>

// #define VBATPIN A13
// #define BUTTON_A 15
// #define BUTTON_B 32
// #define BUTTON_C 14

// Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

// // integer variable to hold current counter value
// int count = 0;
// // integer variable to hold screen state; 0=0ff, 1== on
// int state = 0;

// void setup()
// {
//   pinMode(BUTTON_A, INPUT_PULLUP);
//   pinMode(BUTTON_B, INPUT_PULLUP);
//   pinMode(BUTTON_C, INPUT_PULLUP);
//   Serial.begin(115200);

//   oled.init();
//   oled.setBatteryVisible(true);
// }

// void loop()
// {
//   // clear display
//   oled.clearDisplay();
//   oled.display();

//   if (!digitalRead(BUTTON_A)) state = 1;
//   if (!digitalRead(BUTTON_B)) state = 0;
//   if (!digitalRead(BUTTON_C)) oled.print("C");
//   delay(10);

//   // get the current voltage of the battery from
//   // one of the platform specific functions below
//   float battery = getBatteryVoltage();

//   // update the battery icon
//   oled.setBattery(battery);
//   oled.renderBattery();

//   // print the count value to the OLED
//   oled.print("count: ");
//   oled.println(count);

//   // update the display with the new count
//   if (state == 0) oled.display();

//   // increment the counter by 1
//   count++;

//   // delay 1 second (1 second == 1000 milliseconds)
//   delay(1000);
// }


// //feathers esp32
// float getBatteryVoltage() {
//   float measuredvbat = analogRead(VBATPIN);

//   measuredvbat *= 2;    // we divided by 2, so multiply back
//   measuredvbat *= 3.8;  // Multiply by 3.3V, our reference voltage, 3.8 seems more accurate???
//   measuredvbat /= 4096; // convert to voltage

//   return measuredvbat;
// }