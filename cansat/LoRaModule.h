#ifndef LORAMODULE_H
#define LORAMODULE_H

#include <Arduino.h>
#include <LoRa.h>

// Δομή για πακέτο LoRa που περιέχει το μήνυμα και το RSSI
struct LoRaData {
  String message;
  int rssi;
};

class LoRaModule {
  private:
    int ss;
    int rst;
    int dio0;
    long frequency;
    int syncWord;

  public:
    LoRaModule(int ssPin, int rstPin, int dio0Pin, long freq, int sync);
    void begin();
    void sendString(String message);
    // Λήψη πακέτου που επιστρέφει το μήνυμα και το RSSI
    LoRaData receivePacket();
};

#endif
