#include "Sensors.h"

// Δημιουργία αντικειμένου Sensors
Sensors sensors;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing sensors... Let's go!!!");

    // Αρχικοποίηση αισθητήρων
    sensors.initialize();

    Serial.println("Sensor initialization completed! Yeah!!!!!");
}

void loop() {
    // Ανάγνωση όλων των τιμών
    sensors.read();

    // Εκτύπωση των τιμών ως string
    Serial.print("All Sensor Values: ");
    Serial.println(sensors.to_string().c_str());

    Serial.println("----------------------");
    delay(2000); // Αναμονή 2 δευτερόλεπτα
}
