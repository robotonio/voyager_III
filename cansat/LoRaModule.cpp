  #include "LoRaModule.h"

  LoRaModule::LoRaModule(int ssPin, int rstPin, int dio0Pin, long freq, int sync) {
    ss = ssPin;
    rst = rstPin;
    dio0 = dio0Pin;
    frequency = freq;
    syncWord = sync;
  }

  void LoRaModule::begin() {
    // Ρύθμιση του LoRa transceiver module
    LoRa.setPins(ss, rst, dio0);
    
    // Αντικαταστήστε το LoRa.begin(---E-) με τη συχνότητα της περιοχής σας
    while (!LoRa.begin(frequency)) {
      delay(500);
    }
    // Αλλαγή του sync word για να ταιριάζει με τον δέκτη
    LoRa.setSyncWord(syncWord);
  }

  void LoRaModule::sendString(String message) {
    // Αποστολή πακέτου LoRa στον δέκτη
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
  }

  String LoRaModule::receiveString() {
    String receivedMessage = "";
    // Προσπάθεια ανάλυσης πακέτου
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // Λήψη πακέτου
      while (LoRa.available()) {
        receivedMessage += (char)LoRa.read();
      }
    }
    return receivedMessage;
  }