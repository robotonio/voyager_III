#ifndef BMP280_H
#define BMP280_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

class BMP280 {
  private:
    Adafruit_BMP280 bmp;       // Αντικείμενο BMP280
    float pressureAtLaunch;    // Πίεση στο επίπεδο εκτόξευσης
    float seaLevelPressure;    // Πίεση στο επίπεδο της θάλασσας
    bool debug;
    float currentTemp;     // Αρχική θερμοκρασία (π.χ. 25°C)
    float minTemp;     // Ελάχιστη λογική θερμοκρασία
    float maxTemp;     // Μέγιστη λογική θερμοκρασία
    float currentPressure;
    float minPressure;
    float maxPressure;
    float currentAltitude;
    float minAltitude;

  public:
    // Constructor
    BMP280(float seaLevelPressure = 1013.25, bool debug=false);

    // Αρχικοποίηση
    bool initialize();

    // Μέθοδοι μέτρησης
    float readTemperature();
    float readPressure();
    float readAltitude();

    // Ορισμός πίεσης στο επίπεδο της θάλασσας
    void setSeaLevelPressure(float value);
};

#endif // BMP280_H
