#include "SDModule.h"

SDModule::SDModule(int csPin) {
  cs = csPin;
}

void SDModule::begin() {
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH); // Αρχικά απενεργοποιημένο

  // Αρχικοποίηση SD κάρτας
  if (!SD.begin(cs)) {
    Serial.println("SD card initialization failed!");
  } else {
    Serial.println("SD card initialized successfully!");
  }
}

bool SDModule::writeToFile(String filename, String data) {
  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.println(data);
    file.close();
    Serial.println("Data written to " + filename);
    return true;
  } else {
    Serial.println("Failed to open file " + filename + " for writing");
    return false;
  }
}

String SDModule::readFromFile(String filename) {
  File file = SD.open(filename, FILE_READ);
  String content = "";
  
  if (file) {
    while (file.available()) {
      content += (char)file.read();
    }
    file.close();
    Serial.println("Read from file " + filename);
  } else {
    Serial.println("Failed to open file " + filename);
  }

  return content;
}
