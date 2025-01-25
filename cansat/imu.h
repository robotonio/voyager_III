#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include "GY521.h"
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <math.h>

class IMU {
  private:
    GY521 gy521; // GY521 αισθητήρας
    Adafruit_LIS3MDL lis3mdl; // Μαγνητόμετρο

  public:
    IMU(); // Constructor
    bool initialize(); // Αρχικοποίηση των αισθητήρων
    int16_t readMPU(); // Ανάγνωση δεδομένων από τον MPU6050
    float getYaw(); // Υπολογισμός Yaw
    float getPitch(); // Υπολογισμός Pitch
    float getRoll(); // Υπολογισμός Roll
    void getOrientation(float &yaw, float &roll, float &pitch); // Επιστροφή Yaw, Roll, Pitch
};

#endif // IMU_H
