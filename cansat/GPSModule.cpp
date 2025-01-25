#include "GPSModule.h"

// Constructor
GPSModule::GPSModule(int rxPin, int txPin, int baudRate)
    : rxPin(rxPin), txPin(txPin), baudRate(baudRate), gpsSerial(1), isInitialized(false) {}

// Αρχικοποίηση GPS Module
bool GPSModule::initialize() {
    gpsSerial.begin(baudRate, SERIAL_8N1, rxPin, txPin); // Αρχικοποίηση UART
    isInitialized = true; // Θεωρούμε επιτυχή αρχικοποίηση αν η σειριακή επικοινωνία ξεκινήσει
    return isInitialized;
}

// Ανάγνωση γεωγραφικών συντεταγμένων
std::pair<float, float> GPSModule::getCoordinates() {
    if (!isInitialized) return {0.0, 0.0}; // Επιστροφή μηδενικών αν το GPS δεν έχει αρχικοποιηθεί

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        return {gps.location.lat(), gps.location.lng()};
    }
    return {0.0, 0.0}; // Επιστροφή μηδενικών αν δεν υπάρχουν δεδομένα
}

// Ανάγνωση υψομέτρου
float GPSModule::getAltitude() {
    if (!isInitialized) return 0.0; // Επιστροφή μηδενικού αν το GPS δεν έχει αρχικοποιηθεί

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.altitude.isUpdated()) {
        return gps.altitude.meters();
    }
    return 0.0; // Επιστροφή μηδενικού αν δεν υπάρχουν δεδομένα
}

// Ανάγνωση αριθμού δορυφόρων
int GPSModule::getSatellites() {
    if (!isInitialized) return 0; // Επιστροφή μηδενικού αν το GPS δεν έχει αρχικοποιηθεί

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    return gps.satellites.value();
}

// Ανάγνωση ταχύτητας
float GPSModule::getSpeed() {
    if (!isInitialized) return 0.0; // Επιστροφή μηδενικού αν το GPS δεν έχει αρχικοποιηθεί

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    return gps.speed.kmph();
}

// Ανάγνωση ώρας (UTC)
String GPSModule::getTime() {
    if (!isInitialized) return "00:00:00"; // Επιστροφή μηδενικής ώρας αν το GPS δεν έχει αρχικοποιηθεί

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.time.isValid()) {
        char timeBuffer[16];
        sprintf(timeBuffer, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        return String(timeBuffer);
    }
    return "00:00:00"; // Επιστροφή μηδενικής ώρας αν δεν υπάρχουν δεδομένα
}
