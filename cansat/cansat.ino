#define BUZZER_PIN 8  // GPIO όπου είναι συνδεδεμένο το buzzer

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  playTone(1000, 500);  // 1 KHz για 500ms
  delay(500);
  playTone(1500, 500);  // 1.5 KHz για 500ms
  delay(500);
  playTone(2000, 500);  // 2 KHz για 500ms
  delay(1000);          // Παύση
}

// 🔊 Συνάρτηση που παίζει ήχο σε συγκεκριμένη συχνότητα (Hz) για συγκεκριμένη διάρκεια (ms)
void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration);  // Περιμένει μέχρι να τελειώσει ο ήχος
  noTone(BUZZER_PIN);
}
