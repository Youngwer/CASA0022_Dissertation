#include <MKRWAN.h>
void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  // Wait for Serial Monitor to be opened
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  // Initialize the LoRa modem
  LoRaModem modem;
  // Specify the LoRa band
  if (!modem.begin(EU868)) { // Replace EU868 with the appropriate band for your region
    Serial.println("Failed to start LoRa modem");
    while (1); // Halt if modem fails
  }
  // Retrieve and print the DevEUI
  String devEUI = modem.deviceEUI();
  Serial.print("DevEUI: ");
  Serial.println(devEUI);
}
void loop() {
  // Empty loop, as we only need to retrieve DevEUI once
}