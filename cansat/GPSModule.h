#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <utility> // Για std::pair

class GPSModule {
  private:
    int rxPin;         // RX pin για UART
    int txPin;         // TX pin για UART
    int baudRate;      // Ρυθμός baud
    TinyGPSPlus gps;   // TinyGPS++ αντικείμενο
    HardwareSerial gpsSerial; // UART Serial για το GPS
    bool isInitialized; // Κατάσταση αρχικοποίησης

  public:
    // Constructor
    GPSModule(int rxPin, int txPin, int baudRate = 9600);

    // Αρχικοποίηση GPS Module
    bool initialize();

    // Ανάγνωση γεωγραφικών συντεταγμένων
    std::pair<float, float> getCoordinates();

    // Ανάγνωση υψομέτρου
    float getAltitude();

    // Ανάγνωση αριθμού δορυφόρων
    int getSatellites();

    // Ανάγνωση ταχύτητας
    float getSpeed();

    // Ανάγνωση ώρας (UTC)
    String getTime();
};

#endif // GPSMODULE_H
