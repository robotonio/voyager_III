#include "GPSModule.h"

GPSModule::GPSModule(int rxPin, int txPin, int baudRate, bool debug)
    : rxPin(rxPin), txPin(txPin), baudRate(baudRate),
      isInitialized(false), debug(debug), centerLat(37.98), centerLon(23.35), maxOffsetLat(0.0009), maxOffsetLon(0.00114),
      currentAltitude(1000.0), minAltitude(0.21),
      // Αρχικοποιούμε το SoftwareSerial με (rxPin, txPin)
      gpsSerial(rxPin, txPin)
{
}

// Αρχικοποίηση GPS Module
bool GPSModule::initialize() {
    // Ξεκινάμε το SoftwareSerial στη baudRate του GPS
    gpsSerial.begin(baudRate);

    // Θεωρούμε επιτυχή αρχικοποίηση αν ανοίξει η σειριακή
    isInitialized = true;
    randomSeed(analogRead(A0));
    return isInitialized;
}

// Ανάγνωση γεωγραφικών συντεταγμένων
std::pair<float, float> GPSModule::getCoordinates() {
  if (!debug){ 
    if (!isInitialized) {
        return {0.0f, 0.0f};
    }

    // Διαβάζουμε όσους χαρακτήρες είναι διαθέσιμοι από το GPS
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    // Αν η τοποθεσία έχει ενημερωθεί, επιστρέφουμε lat/lng
    if (gps.location.isUpdated()) {
        return { gps.location.lat(), gps.location.lng() };
    }
    // Αλλιώς επιστροφή μηδενικών
    return {0.0f, 0.0f};
  }
  else{
    // Παράγουμε τυχαίο offset στο εύρος [-1..+1], μετά το πολλαπλασιάζουμε με maxOffsetLat
    float offsetLat = (random(-1000, 1001) / 1000.0) * maxOffsetLat;
    float offsetLon = (random(-1000, 1001) / 1000.0) * maxOffsetLon;

    // Υπολογίζουμε τις νέες συντεταγμένες
    float newLat = centerLat + offsetLat;
    float newLon = centerLon + offsetLon;
    return {newLat, newLon};
  }
}

// Ανάγνωση υψομέτρου
float GPSModule::getAltitude() {
  if (!debug){
    if (!isInitialized) return 0.0f;

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.altitude.isUpdated()) {
        return gps.altitude.meters();
    }
    return 0.0f;
  }
  else {
    if (currentAltitude > minAltitude){
      float randomChange = (random(0, 2001) / 1000.0) - 1.0;
      currentAltitude += randomChange - 10;
    }
    if (currentAltitude < minAltitude) currentAltitude = minAltitude;
    return currentAltitude;
  }
}

// Ανάγνωση αριθμού δορυφόρων
int GPSModule::getSatellites() {
  if (!debug){
    if (!isInitialized) return 0;

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    return gps.satellites.value();
  }
  return 13;
}

// Ανάγνωση ταχύτητας
float GPSModule::getSpeed() {
  if (!debug){
    if (!isInitialized) return 0.0f;

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    return gps.speed.kmph();
  }
  else {
    float randomChange = (random(0, 2001) / 1000.0) - 1.0;
    return 10.0 + randomChange;
  }
}

// Ανάγνωση ώρας (UTC)
String GPSModule::getTime() {
  if (!debug){
    if (!isInitialized) return "00:00:00";

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.time.isValid()) {
        char timeBuffer[16];
        sprintf(timeBuffer, "%02d:%02d:%02d",
                gps.time.hour(),
                gps.time.minute(),
                gps.time.second());
        return String(timeBuffer);
    }
    return "00:00:00";
  }
  return "11:53:45";
}

void GPSModule::update() {
  if (!isInitialized) return;
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
}