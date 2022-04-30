#include <Arduino.h>

#include <io/lora/communication.hpp>

void setup() {
  namespace io = core::types::io;

  Serial.begin(115200);
  Serial.println("initialized");

  if (not core::network::lora::initialize(5, 14, 2)) {
    Serial.println("Failed!");
    unlimited_loop {};
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}