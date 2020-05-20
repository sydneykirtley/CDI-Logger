#include <BluetoothSerial.h> //this library allows use of the Bluetooth serial features of the esp32

BluetoothSerial SerialBT; //rename the Bluetooth serial features to be more recognizable

void setup() {
  Serial.begin(115200); //start serial communication with the Arduino serial monitor
  Serial1.begin(115200); //start serial communication with the CDI port (defaults to the TX/RX pins)
  SerialBT.register_callback(callback); //call the callback function -- has to to do with the connection over Bluetooth

  if(!SerialBT.begin("ESP32")){
    Serial.println("An error occurred initializing Bluetooth"); //if the serial communication with Bluetooth doesn't begin, print this to Arduino monitor
  }else{
    Serial.println("Bluetooth initialized"); //if the serial communication with Bluetooth does begin, print this to the Arduino monitor
  }
}

  void loop (){
    while(1){
      if (Serial1.available()) {
          SerialBT.write(Serial1.read()); //if there is data available over CDI, write it to the Bluetooth monitor
      }
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