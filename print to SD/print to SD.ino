#include <SPI.h>
#include <SD.h>

File logfile;  //setup and placeholder for SD card file

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); //start serial communication with the CDI port (defaults to the TX/RX pins)
  Serial.println("CDI SD logger test");
  
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
        logfile.print(Serial.readstring());
        logfile.flush();
    
    while(1){
      if (Serial1.available()) {
          SerialBT.write(Serial1.read()); //if there is data available over CDI, write it to the Bluetooth monitor
      }
    }

}