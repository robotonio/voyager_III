#ifndef LORAMODULE_H
#define LORAMODULE_H

#include <Arduino.h>
#include <LoRa.h>

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
    String receiveString();
};

#endif