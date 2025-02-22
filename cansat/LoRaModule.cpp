#include "LoRaModule.h"

LoRaModule::LoRaModule(int ssPin, int rstPin, int dio0Pin, long freq, int sync) {
  ss = ssPin;
  rst = rstPin;
  dio0 = dio0Pin;
  frequency = freq;
  syncWord = sync;
}

void LoRaModule::begin() {
  // Ρύθμιση των pins του LoRa module
  LoRa.setPins(ss, rst, dio0);
  
  // Εκκίνηση με τη σωστή συχνότητα (π.χ. 868E6 για Ευρώπη)
  while (!LoRa.begin(frequency)) {
    delay(500);
  }
  // Ορισμός του sync word ώστε να ταιριάζει με τον αποστολέα
  LoRa.setSyncWord(syncWord);
}

void LoRaModule::sendString(String message) {
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}

LoRaData LoRaModule::receivePacket() {
  LoRaData data;
  data.message = "";
  data.rssi = 0;
  
  // Έλεγχος για ληφθέν πακέτο
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      data.message += (char)LoRa.read();
    }
    data.rssi = LoRa.packetRssi();
  }
  return data;
}
