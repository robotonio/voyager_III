#include "Buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin, bool activeHigh) 
  : _pin(buzzerPin), _activeHigh(activeHigh) {}

void Buzzer::begin() {
  pinMode(_pin, OUTPUT);
  off();  // Ξεκινάμε σε κατάσταση off
}

void Buzzer::on() {
  // Αν το buzzer είναι activeHigh, στέλνουμε HIGH για να ηχήσει
  // Διαφορετικά στέλνουμε LOW.
  if (_activeHigh) {
    digitalWrite(_pin, HIGH);
  } else {
    digitalWrite(_pin, LOW);
  }
}

void Buzzer::off() {
  // Αν το buzzer είναι activeHigh, στέλνουμε LOW για να σιγήσει
  // Διαφορετικά στέλνουμε HIGH.
  if (_activeHigh) {
    digitalWrite(_pin, LOW);
  } else {
    digitalWrite(_pin, HIGH);
  }
}

void Buzzer::beep(unsigned long durationMs) {
  on();
  delay(durationMs);
  off();
}
