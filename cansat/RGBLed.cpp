#include "RGBLed.h"

// Constructor: Αποθηκεύει τα GPIOs για τα κανάλια R, G, B
RGBLed::RGBLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
    : _redPin(redPin), _greenPin(greenPin), _bluePin(bluePin) {}

// Αρχικοποίηση των pins ως έξοδοι
void RGBLed::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    turnOff(); // Ξεκινά με το LED σβηστό
}

// Θέτει το χρώμα με PWM
void RGBLed::setColor(uint8_t r, uint8_t g, uint8_t b) {
    analogWrite(_redPin, r);
    analogWrite(_greenPin, g);
    analogWrite(_bluePin, b);
}

// Σβήνει το LED (PWM = 0)
void RGBLed::turnOff() {
    analogWrite(_redPin, 0);
    analogWrite(_greenPin, 0);
    analogWrite(_bluePin, 0);
}
