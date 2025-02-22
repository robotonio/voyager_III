#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include "GY521.h"
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <math.h>

class IMU {
  private:
    Adafruit_LIS3MDL lis3mdl; // Μαγνητόμετρο
    int16_t groll;
    int16_t gpitch;
    int16_t gyaw;
    float lyaw;
    uint8_t readRegister(uint8_t devAddr, uint8_t regAddr);
    void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    bool debug;

  public:
    IMU(bool debug=false); // Constructor
    bool initialize(); // Αρχικοποίηση των αισθητήρων
    void read(); // Ανάγνωση δεδομένων από τον MPU6050
    float getYaw(); // Υπολογισμός Yaw
    float getPitch(); // Υπολογισμός Pitch
    float getRoll(); // Υπολογισμός Roll
    void getOrientation(float &yaw, float &roll, float &pitch); // Επιστροφή Yaw, Roll, Pitch
};

#endif // IMU_H
