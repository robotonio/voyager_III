#include <Arduino.h>
#include "LoRaModule.h"

// Ορίστε εδώ τα pin και τις ρυθμίσεις που χρησιμοποιείτε
#define LORA_SS_PIN      5
#define LORA_RST_PIN     14
#define LORA_DIO0_PIN    2

// Αντικαταστήστε με τη συχνότητα λειτουργίας της περιοχής σας
// π.χ. 433E6 (433 MHz), 866E6 (866 MHz), 868E6 (868 MHz), 915E6 (915 MHz) 
#define LORA_FREQUENCY   868E6  

// Ορίστε το Sync Word (π.χ. 0x12) ώστε να ταιριάζει μεταξύ πομπού και δέκτη
#define LORA_SYNC_WORD   0x12

// Δημιουργία αντικειμένου LoRaModule
LoRaModule lora(LORA_SS_PIN, LORA_RST_PIN, LORA_DIO0_PIN, LORA_FREQUENCY, LORA_SYNC_WORD);

void setup() {
  // Αρχικοποίηση σειριακής επικοινωνίας για debug
  Serial.begin(9600);
  while (!Serial) {
    ; // Περιμένετε μέχρι να ανοίξει το σειριακό (απαραίτητο μόνο σε Arduino Leonardo/Micro)
  }

  Serial.println("Ξεκινάει η αρχικοποίηση του LoRa...");

  // Ξεκινάμε το LoRa module
  lora.begin();
  
  Serial.println("Η αρχικοποίηση του LoRa ολοκληρώθηκε επιτυχώς!");
}

void loop() {
  // 1. Αποστολή μηνύματος
  String msgToSend = "Hello from Arduino!";
  lora.sendString(msgToSend);
  Serial.print("Απεστάλη μήνυμα: ");
  Serial.println(msgToSend);

  // 2. Αναμονή για λίγο
  delay(2000);

  // 3. Έλεγχος για λήψη μηνύματος
  String received = lora.receiveString();
  if (received.length() > 0) {
    Serial.print("Ελήφθη μήνυμα: ");
    Serial.println(received);
  }

  // 4. Μικρή καθυστέρηση για να μην κατακλύζουμε το δίκτυο
  delay(1000);
}
