#include <SPI.h>
#include <LoRa.h>
#include "LoRaModule.h"
#include <LiquidCrystal_I2C.h>

//============================================================//
// Standby Mode: "0", LoRa Working: "0LW"                     //
// Basic Mission: "1"                                         //
// VTX On: "2"                                                //
// Find Me Mode: "3", Buzzer: "3B"                            //    
//============================================================//


// Ορισμός των pins σύμφωνα με το υλικό σας
#define SS_PIN 5
#define RST_PIN 14
#define DIO0_PIN 2

// Set button pins
#define BUTTON 27
// define LED pin:
//******************//

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 4;

// CanSat Mode
String cansat_mode = "0";


// Δημιουργία αντικειμένου LoRaModule με συχνότητα 868E6 και sync word 0xF3
LoRaModule lora(SS_PIN, RST_PIN, DIO0_PIN, 433E6, 0xF3);

// Δημιουργία αντικειμένου LCD
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

void setup() {
    Serial.begin(9600);
    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
    
    // Set button mode to pullup
    pinMode(BUTTON, INPUT_PULLUP);
    // pinMode(led_pin, OUTPUT);
    // digitalWrite(led_pin, LOW);

    while (!Serial);
    Serial.println("LoRa Receiver using LoRaModule");

    lora.begin();
    Serial.println("LoRa Initializing OK!");
    lcd.setCursor(0,0);
    lcd.print("LoRa OK!");
    delay(5000);
    lcd.clear();

}

void loop() {

int button_state = digitalRead(BUTTON);

// Standby Mode
  if (cansat_mode == "0"){
    Serial.println("Standby Mode");
    lcd.setCursor(0, 3);
    lcd.print("STANDBY");

    if (button_state == LOW){
      lora.sendString("1"); // Message to cansat to start Basic Mission
    }
  }

// Basic Mission
  if (cansat_mode == "1"){
    Serial.println("Basic Missiopn");
    lcd.setCursor(0, 3);
    lcd.print("Basic Mission");

    if (button_state == LOW){
      lora.sendString("2"); // Messagew to cansat to turn on VTX
    }
  }

// VTX On
  if (cansat_mode == "2"){
    Serial.println("VTX On");
    lcd.setCursor(0, 3);
    lcd.print("VTX On");

    if (button_state == LOW){
      lora.sendString("3"); // Message to cansat to enter Find Me Mode
    }
  }

// Find Me Mode
  if (cansat_mode == "3"){
    Serial.println("Find Me Mode");
    lcd.setCursor(0, 3);
    lcd.print("FIND ME");

    if (button_state == LOW){
      lora.sendString("3B"); // Message to cansat to turn buzzer on
      Serial.println("buzzer on");
      lcd.setCursor(0, 1);
      lcd.print("BUZZER ON");
      delay(3000);
    }
  }


  LoRaData packet = lora.receivePacket();
  
  // Αν υπάρχει ληφθέν πακέτο, εμφανίζουμε το μήνυμα και το RSSI
  if (packet.message.length() > 0) {
    Serial.print("Received packet '");
    Serial.print(packet.message);
    Serial.print("' with RSSI ");
    Serial.println(packet.rssi);
  }

  //LoRa working on Standby Mode
  if (packet.message == "0LW"){
    Serial.println("LoRa OK!");
    lcd.setCursor(0,0);
    lcd.print("LoRa OK!");
    delay(3000);
    lcd.clear();
  }

  if (packet.message == "0"){
    cansat_mode = "0"; // Standby Mode
  }
  if (packet.message == "1"){
    cansat_mode = "1"; // Basic Mission
  }
  if (packet.message == "2"){
    cansat_mode = "2"; // VTX On 
  }  
  if (packet.message == "3"){
    cansat_mode = "3"; // Find Me Mode 
  }

}
