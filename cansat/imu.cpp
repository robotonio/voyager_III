#include "IMU.h"

// Constructor
IMU::IMU() : gy521(0x68) {}

// Αρχικοποίηση αισθητήρων
bool IMU::initialize() {
    bool gy521Initialized = gy521.begin(); // Αρχικοποίηση GY521
    bool lis3mdlInitialized = lis3mdl.begin_I2C(); // Αρχικοποίηση LIS3MDL

    if (gy521Initialized && lis3mdlInitialized) {
        return true; // Επιτυχής αρχικοποίηση
    } else {
        Serial.println("IMU initialization failed!");
        return false; // Αποτυχία αρχικοποίησης
    }
}

// Ανάγνωση δεδομένων από τον MPU6050
int16_t IMU::readMPU() {
    return gy521.read();
}

// Υπολογισμός Yaw
float IMU::getYaw() {
    sensors_event_t mag;
    lis3mdl.getEvent(&mag);

    float yaw = atan2(mag.magnetic.y, mag.magnetic.x) * 180 / M_PI;
    if (yaw < 0) yaw += 360;

    return yaw;
}

// Υπολογισμός Pitch
float IMU::getPitch() {
    gy521.readAccel();
    float pitch = atan2(gy521.getAccelY(), sqrt(gy521.getAccelX() * gy521.getAccelX() + gy521.getAccelZ() * gy521.getAccelZ())) * 180 / M_PI;
    return pitch;
}

// Υπολογισμός Roll
float IMU::getRoll() {
    gy521.readAccel();
    float roll = atan2(-gy521.getAccelX(), gy521.getAccelZ()) * 180 / M_PI;
    return roll;
}

// Επιστροφή Yaw, Roll, Pitch
void IMU::getOrientation(float &yaw, float &roll, float &pitch) {
    yaw = getYaw();
    roll = getRoll();
    pitch = getPitch();
}
