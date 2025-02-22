#include <Wire.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include "imu.h"
#define MPU_ADDR   0x68 // Συνήθης διεύθυνση για τον MPU-6050 (GY-521)
#define WHO_AM_I   0x75 // Καταχωρητής για WHO_AM_I (περιμένουμε τιμή 0x68)

Adafruit_LIS3MDL lis3mdl = Adafruit_LIS3MDL();
int16_t groll;
int16_t gpitch;
int16_t gyaw;
float lyaw;

// Constructor
IMU::IMU(bool debug)
  : debug(debug)
{}

// Αρχικοποίηση αισθητήρων
bool IMU::initialize() {
  //// ITG/IMU ////
  Wire.begin();
  Serial.println("=== MPU/ITG Sensor Check ===");
  // 1. Έλεγχος του WHO_AM_I register
  uint8_t whoAmI = readRegister(MPU_ADDR, WHO_AM_I);
  Serial.print("WHO_AM_I register = 0x");
  Serial.println(whoAmI, HEX);
  if (whoAmI == MPU_ADDR) {
    Serial.println("MPU-6050/ITG αισθητήρας βρέθηκε!");
  } else {
    Serial.println("Αποτυχία εντοπισμού MPU/ITG! Ελέγξτε τη συνδεσμολογία ή τη διεύθυνση I2C.");
    // Εναλλακτικά, μπορούμε να σταματήσουμε εδώ.
    // while(1);
  }
  // 2. Ξυπνάμε τον αισθητήρα (Power Management Register 0x6B)
  // Θέτουμε το bit SLEEP = 0
  writeRegister(MPU_ADDR, 0x6B, 0x00);
  Serial.println("Ξεκλειδώθηκε ο αισθητήρας (sleep off).");
  Serial.println("=============================");

  //// LIS3MDL ////
  // Αρχικοποίηση σε I2C
  if (!lis3mdl.begin_I2C()) {
    Serial.println("Failed to find LIS3MDL chip! Check wiring or I2C address.");
    while (1) { delay(10); }
  }
  Serial.println("LIS3MDL Found!");
  lis3mdl.configInterrupt(false, false, false,
                          false, false, false);
  Serial.println("LIS3MDL setup complete");

  randomSeed(analogRead(A0));

  return true;
}

uint8_t IMU::readRegister(uint8_t devAddr, uint8_t regAddr) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.endTransmission(false);
  Wire.requestFrom(devAddr, (uint8_t)1, true); 
  return Wire.read();
}

void IMU::writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.write(data);
  Wire.endTransmission(true);
}

// Ανάγνωση δεδομένων από τον MPU6050
void IMU::read() {
  //// ITG/IMU ////
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43);               // Πρώτος καταχωρητής γυροσκοπίου
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); // 6 bytes
  groll = (Wire.read() << 8) | Wire.read();
  gpitch = (Wire.read() << 8) | Wire.read();
  gyaw = (Wire.read() << 8) | Wire.read();
  
  //// LIS3MDL ////
  sensors_event_t event;
  lis3mdl.getEvent(&event);
  lyaw = event.magnetic.z;
}

// Υπολογισμός Yaw
float IMU::getYaw() {
  if (!debug){
    lyaw = fmod(lyaw, 360.0);
    return lyaw;
  }
  else {
    int randint = random(0, 720);
    randint -= 360;
    return randint;
  }
}

// Υπολογισμός Pitch
float IMU::getPitch() {
  if (!debug){
    float pitch = gpitch / 131;
    return pitch;
  }
  else {
    int randint = random(0, 720);
    randint -= 360;
    return randint;
  }
}

// Υπολογισμός Roll
float IMU::getRoll() {
  if (!debug){
    float roll = groll / 131;
    return roll;
  }
  else {
    int randint = random(0, 720);
    randint -= 360;
    return randint;
  }
}

// Επιστροφή Yaw, Roll, Pitch
void IMU::getOrientation(float &yaw, float &roll, float &pitch) {
    yaw = getYaw();
    roll = getRoll();
    pitch = getPitch();
}
