#include "BMP280.h"

// Constructor
BMP280::BMP280(float seaLevelPressure) 
    : seaLevelPressure(seaLevelPressure), pressureAtLaunch(0.0) {}

// Αρχικοποίηση BMP280
bool BMP280::initialize() {
    if (!bmp.begin(0x77)) { // Χρήση της δοκιμασμένης διεύθυνσης 0x77
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        return false;
    }

    // Ρύθμιση BMP280
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,      // Oversampling για θερμοκρασία
                    Adafruit_BMP280::SAMPLING_X16,     // Oversampling για πίεση
                    Adafruit_BMP280::FILTER_X16,       // Φιλτράρισμα
                    Adafruit_BMP280::STANDBY_MS_500);  // Standby time

    // Καταγραφή πίεσης στο επίπεδο εκτόξευσης
    pressureAtLaunch = bmp.readPressure() / 100.0F; // Μετατροπή σε hPa
    if (pressureAtLaunch < 900.0) { // Ελέγχουμε για reset ψηλά
        Serial.println("Detected reset in flight! Using sea-level pressure.");
        pressureAtLaunch = seaLevelPressure;
    }

    Serial.print("Pressure at launch: ");
    Serial.print(pressureAtLaunch);
    Serial.println(" hPa");

    return true;
}

// Μέθοδος για ανάγνωση θερμοκρασίας
float BMP280::readTemperature() {
    return bmp.readTemperature();
}

// Μέθοδος για ανάγνωση πίεσης
float BMP280::readPressure() {
    return bmp.readPressure() / 100.0F; // Επιστροφή σε hPa
}

// Μέθοδος για ανάγνωση υψομέτρου
float BMP280::readAltitude() {
    return bmp.readAltitude(pressureAtLaunch);
}

// Μέθοδος για ορισμό πίεσης στο επίπεδο της θάλασσας
void BMP280::setSeaLevelPressure(float value) {
    seaLevelPressure = value;
}
