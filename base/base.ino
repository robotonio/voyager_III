#include <SPI.h>
#include <LoRa.h>
#include "LoRaModule.h"

// Ορισμός των pins σύμφωνα με το υλικό σας
#define SS_PIN 5
#define RST_PIN 14
#define DIO0_PIN 2

// Δημιουργία αντικειμένου LoRaModule με συχνότητα 868E6 και sync word 0xF3
LoRaModule lora(SS_PIN, RST_PIN, DIO0_PIN, 433E6, 0xF3);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver using LoRaModule");

  lora.begin();
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  LoRaData packet = lora.receivePacket();
  
  // Αν υπάρχει ληφθέν πακέτο, εμφανίζουμε το μήνυμα και το RSSI
  if (packet.message.length() > 0) {
    Serial.print("Received packet '");
    Serial.print(packet.message);
    Serial.print("' with RSSI ");
    Serial.println(packet.rssi);
  }
}
