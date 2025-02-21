#include "LoRaModule.h"

LoRaModule::LoRaModule(int ssPin, int rstPin, int dio0Pin, long freq, int sync) {
  ss = ssPin;
  rst = rstPin;
  dio0 = dio0Pin;
  frequency = freq;
  syncWord = sync;
}

void LoRaModule::begin() {
  // Ορίζουμε το CS του LoRa αρχικά ανενεργό
  pinMode(ss, OUTPUT);
  digitalWrite(ss, HIGH);

  Serial.println("Initializing LoRa...");
  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa initialization failed. Retrying...");
    delay(500);
  }
  LoRa.setSyncWord(syncWord);
  Serial.println("LoRa Initialized Successfully!");
}

void LoRaModule::sendString(String message, int sdCS) {
  // Απενεργοποιούμε το SD πριν ενεργοποιήσουμε το LoRa
  digitalWrite(sdCS, HIGH);
  digitalWrite(ss, LOW); // Ενεργοποίηση LoRa

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  digitalWrite(ss, HIGH); // Απενεργοποίηση LoRa
}

String LoRaModule::receiveString(int sdCS) {
  String receivedMessage = "";

  // Απενεργοποιούμε το SD πριν χρησιμοποιήσουμε το LoRa
  digitalWrite(sdCS, HIGH);
  digitalWrite(ss, LOW); // Ενεργοποίηση LoRa

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      receivedMessage += (char)LoRa.read();
    }
  }

  digitalWrite(ss, HIGH); // Απενεργοποίηση LoRa
  return receivedMessage;
}
