#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>

class RGBLed {
public:
    // Constructor: ορίζει τα GPIOs για Red, Green, Blue
    RGBLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);

    // Αρχικοποίηση των pins
    void begin();

    // Θέτει το χρώμα RGB (0-255 για κάθε κανάλι)
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    // Σβήνει το LED
    void turnOff();

private:
    uint8_t _redPin, _greenPin, _bluePin; // GPIOs για τα LED κανάλια
};

#endif
