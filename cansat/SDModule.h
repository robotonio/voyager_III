#ifndef SDMODULE_H
#define SDMODULE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class SDModule {
  private:
    int cs; // Chip Select Pin για το SD module

  public:
    SDModule(int csPin);
    void begin();
    bool writeToFile(String filename, String data);
    String readFromFile(String filename);
};

#endif
