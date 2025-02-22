#include <SPI.h>
#include <LoRa.h>
#include "LoRaModule.h"
#include "CanSatData.h"

// Ορισμός των pins για το LoRa module
#define SS_PIN 5
#define RST_PIN 6
#define DIO0_PIN 4

// Δημιουργία αντικειμένου LoRaModule με συχνότητα 868 MHz και sync word 0xF3
LoRaModule lora(SS_PIN, RST_PIN, DIO0_PIN, 433E6, 0xF3);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender - Sending dump data using CanSatData");
  
  // Εκκίνηση του LoRa module μέσω της κλάσης
  lora.begin();
  Serial.println("LoRa Initialized!");
}

void loop() {
  // Δημιουργία dummy δεδομένων μέσω της CanSatData
  CanSatData dumpData = CanSatData::createDump();
  // Μετατροπή των δεδομένων σε string (σε μορφή CSV περικλεισμένη από #)
  String dumpString = dumpData.toString();

  // Εμφάνιση στο Serial Monitor για debug
  Serial.print("Sending dump data: ");
  Serial.println(dumpString);

  // Αποστολή των δεδομένων μέσω του LoRaModule
  lora.sendString(dumpString);

  // Αναμονή 5 δευτερολέπτων πριν την επόμενη αποστολή
  delay(5000);
}
