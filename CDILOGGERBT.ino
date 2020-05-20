#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  SerialBT.register_callback(callback);

  if(!SerialBT.begin("ESP32")){
    Serial.println("An error occurred initializing Bluetooth");
  }else{
    Serial.println("Bluetooth initialized");
  }
}

  while(1){
    if (Serial1.available()) {
        SerialBT.write(Serial1.read());
    }
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    SerialBT.println("Client Connected");
  }

  if(event == ESP_SPP_CLOSE_EVT ){
    SerialBT.println("Client disconnected");
  }
}
//end