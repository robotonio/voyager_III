#include "BMP280.h"

// Constructor
BMP280::BMP280(float seaLevelPressure, bool debug) 
    : seaLevelPressure(seaLevelPressure), pressureAtLaunch(0.0), debug(debug), currentTemp(25.0), minTemp(10.0), maxTemp(35.0),
      currentPressure(seaLevelPressure), minPressure(980.0), maxPressure(1050.0), currentAltitude(1000.0), minAltitude(0.21) {}

// Αρχικοποίηση BMP280
bool BMP280::initialize() {
    if (!bmp.begin(0x76)) { // Χρήση της δοκιμασμένης διεύθυνσης 0x77
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
    randomSeed(analogRead(A0));

    return true;
}

// Μέθοδος για ανάγνωση θερμοκρασίας
float BMP280::readTemperature() {
  if(!debug) return bmp.readTemperature();
  else {
    // Παράγουμε μια τυχαία μεταβολή στο εύρος [-0.5, +0.5]
    // random(0,1001) δίνει ακέραιο στο [0..1000]
    // διαιρώντας διά 1000.0 γίνεται [0..1], μετά -0.5 => [-0.5..+0.5]
    float randomChange = (random(0, 1001) / 1000.0) - 0.5;

    // Ενημερώνουμε την τρέχουσα θερμοκρασία
    currentTemp += randomChange;

    // Περιορισμός στο [minTemp, maxTemp]
    if (currentTemp < minTemp) {
      currentTemp = minTemp;
    } else if (currentTemp > maxTemp) {
      currentTemp = maxTemp;
    }
    return currentTemp;
  }
}

// Μέθοδος για ανάγνωση πίεσης
float BMP280::readPressure() {
  if (!debug) return bmp.readPressure() / 100.0F; // Επιστροφή σε hPa
  else {
    // Παράγουμε τυχαία μεταβολή στο εύρος [-0.5, +0.5], 
    // μπορείς να το αλλάξεις όπως θες (π.χ. [-1..+1]).
    float randomChange = (random(0, 1001) / 1000.0) - 0.5;

    // Προσαρμόζουμε την πίεση
    currentPressure += randomChange;

    // Περιορισμός στο [minPressure, maxPressure]
    if (currentPressure < minPressure) {
      currentPressure = minPressure;
    } else if (currentPressure > maxPressure) {
      currentPressure = maxPressure;
    }
    return currentPressure;
  }
}

// Μέθοδος για ανάγνωση υψομέτρου
float BMP280::readAltitude() {
  if (!debug) return bmp.readAltitude(pressureAtLaunch);
  else {
    if (currentAltitude > minAltitude){
      float randomChange = (random(0, 2001) / 1000.0) - 1.0;
      currentAltitude += randomChange - 10;
    }
    if (currentAltitude < minAltitude) currentAltitude = minAltitude;
    return currentAltitude;
  }
}

// Μέθοδος για ορισμό πίεσης στο επίπεδο της θάλασσας
void BMP280::setSeaLevelPressure(float value) {
    seaLevelPressure = value;
}
