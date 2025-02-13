#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

/**
 * Κλάση για τον έλεγχο ενός buzzer (παθητικού ή ενεργού).
 * Έχει τη δυνατότητα:
 *   - να ενεργοποιηθεί (on)
 *   - να απενεργοποιηθεί (off)
 *   - να κάνει beep (on -> αναμονή -> off)
 */
class Buzzer {
  private:
    uint8_t _pin;           // Το pin που είναι συνδεδεμένο το buzzer
    bool _activeHigh;       // True αν το buzzer ενεργοποιείται με HIGH, false αν ενεργοποιείται με LOW

  public:
    // Constructor
    // buzzerPin: το pin του Arduino όπου συνδέεται το buzzer
    // activeHigh: αν το buzzer ενεργοποιείται με HIGH (true) ή με LOW (false)
    Buzzer(uint8_t buzzerPin, bool activeHigh = true);

    // Αρχικοποίηση buzzer
    void begin();

    // Ενεργοποίηση (π.χ. HIGH στο pin αν activeHigh = true)
    void on();

    // Απενεργοποίηση (π.χ. LOW στο pin αν activeHigh = true)
    void off();

    // Απλή συνάρτηση beep για συγκεκριμένη διάρκεια σε milliseconds
    void beep(unsigned long durationMs);
};

#endif
