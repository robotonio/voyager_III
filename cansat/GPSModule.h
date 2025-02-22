#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <utility>  // για std::pair

class GPSModule {
public:
  GPSModule(int rxPin, int txPin, int baudRate, bool debug=false);

  bool initialize();

  // Βασικές μετρήσεις
  std::pair<float, float> getCoordinates();
  float getAltitude();
  int   getSatellites();
  float getSpeed();
  String getTime();
  void update();

private:
  int rxPin;
  int txPin;
  int baudRate;
  bool isInitialized;
  bool debug;
  float centerLat;  
  float centerLon;
  float maxOffsetLat;
  float maxOffsetLon;
  float currentAltitude;
  float minAltitude;

  // Χρησιμοποιούμε SoftwareSerial για το GPS
  SoftwareSerial gpsSerial;

  // TinyGPSPlus αντικείμενο για parse NMEA
  TinyGPSPlus gps;
};

#endif
