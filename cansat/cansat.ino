#include <HardwareSerial.h>

HardwareSerial gpsSerial(1);
#define RX_PIN 43
#define TX_PIN 44
#define GPS_BAUD 9600

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.println("Reading GPS Raw Data...");
}

void loop() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        Serial.print(c); // Εκτύπωση ακατέργαστων NMEA δεδομένων
    }
}
