#include <SPI.h>
#include "LoRaModule.h"
#include "Sensors.h"

// Ορισμός των pins που χρησιμοποιούνται από το LoRa transceiver module
#define SS_PIN 7
#define RST_PIN 18
#define DIO0_PIN 38
#define FREQUENCY 433E6
#define SYNC_WORD 0xF3

// Δημιουργία αντικειμένων για LoRa και Sensors
LoRaModule lora(SS_PIN, RST_PIN, DIO0_PIN, FREQUENCY, SYNC_WORD);
Sensors sensors;

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Περιμένει να ξεκινήσει το Serial Monitor
  Serial.println("Initializing sensors...");

  // Αρχικοποίηση αισθητήρων
  sensors.initialize();
  Serial.println("Sensor initialization completed!");

  // Αρχικοποίηση LoRa
  Serial.println("LoRa Initializing...");
  lora.begin();
  Serial.println("LoRa Initialized Successfully!");
}

void loop() {
  // Ανάγνωση όλων των τιμών από τους αισθητήρες
  sensors.read();
  std::string sensorDataStd = sensors.to_string();
  String sensorData = String(sensorDataStd.c_str());

  // Αποστολή δεδομένων μέσω LoRa
  lora.sendString(sensorData);
  Serial.println("Sent: " + sensorData);

  Serial.println("----------------------");
  delay(2000); // Αναμονή 2 δευτερόλεπτα πριν την επόμενη αποστολή

  // Λήψη δεδομένων μέσω LoRa
  String receivedMessage = lora.receiveString();
  if (receivedMessage != "") {
    Serial.print("Received packet: '");
    Serial.print(receivedMessage);
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  } else {
    Serial.println("No new messages received.");
  }

  delay(1000); // Μικρή παύση πριν την επόμενη λήψη
}
