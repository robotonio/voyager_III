#include <SPI.h>
#include "LoRaModule.h"
#include "Sensors.h"

// Ορισμός των pins που χρησιμοποιούνται από το transceiver module
#define SS_PIN 7
#define RST_PIN 18
#define DIO0_PIN 38
#define FREQUENCY 433E6
#define SYNC_WORD 0xF3

LoRaModule lora(SS_PIN, RST_PIN, DIO0_PIN, FREQUENCY, SYNC_WORD);
Sensors sensors;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Initializing sensors... Let's go!!!");

  // Αρχικοποίηση αισθητήρων
  sensors.initialize();
  Serial.println("Sensor initialization completed! Yeah!!!!!");

  // Αρχικοποίηση LoRa
  Serial.println("LoRa Initializing...");
  lora.begin();
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // Ανάγνωση όλων των τιμών από τους αισθητήρες
  sensors.read();
  std::string sensorDataStd = sensors.to_string();
  String sensorData = String(sensorDataStd.c_str());

  // Αποστολή δεδομένων μέσω LoRa
  lora.sendString(sensorData);
  Serial.println("Sent: " + sensorData);

  // Λήψη δεδομένων μέσω LoRa
  String receivedMessage = lora.receiveString();
  if (receivedMessage != "") {
    Serial.print("Received packet '");
    Serial.print(receivedMessage);
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  } else {
    Serial.println("Received Nothing!");
  }

  delay(1000); // Αναμονή 1 δευτερόλεπτο
}