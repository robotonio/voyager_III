#include "Sensors.h"

// Constructor
Sensors::Sensors()
    : bmp280(1013.25),
      imu(),
      gpsModule(44, 43, 9600),
      altitude(0.0), temperature(0.0), pressure(0.0),
      latitude(0.0), longitude(0.0), satellites(0), time("00:00:00"),
      pitch(0.0), roll(0.0), yaw(0.0),
      bmpInitialized(false), imuInitialized(false), gpsInitialized(false) {}

// Αρχικοποίηση αισθητήρων
void Sensors::initialize() {
    bmpInitialized = bmp280.initialize();
    imuInitialized = imu.initialize();
    gpsInitialized = gpsModule.initialize();

    if (!bmpInitialized) Serial.println("BMP280 initialization failed!");
    if (!imuInitialized) Serial.println("IMU initialization failed!");
    if (!gpsInitialized) Serial.println("GPSModule initialization failed!");
}

// Μέθοδος για ανάγνωση όλων των τιμών
void Sensors::read() {
    if (bmpInitialized) {
        altitude = bmp280.readAltitude();
        temperature = bmp280.readTemperature();
        pressure = bmp280.readPressure();
    }

    if (gpsInitialized) {
        auto coordinates = gpsModule.getCoordinates();
        latitude = coordinates.first;
        longitude = coordinates.second;
        satellites = gpsModule.getSatellites();
        time = gpsModule.getTime();
    }

    if (imuInitialized) {
        pitch = imu.getPitch();
        roll = imu.getRoll();
        yaw = imu.getYaw();
    }
}

// Getters
float Sensors::getAltitude() const { return altitude; }
float Sensors::getTemperature() const { return temperature; }
float Sensors::getPressure() const { return pressure; }

float Sensors::getLatitude() const { return latitude; }
float Sensors::getLongitude() const { return longitude; }
int Sensors::getSatellites() const { return satellites; }
String Sensors::getTime() const { return time; }

float Sensors::getPitch() const { return pitch; }
float Sensors::getRoll() const { return roll; }
float Sensors::getYaw() const { return yaw; }

// Μέθοδος to_string για επιστροφή όλων των τιμών
std::string Sensors::to_string() const {
    char buffer[200];
    sprintf(buffer, "%.1f,%.1f,%.1f,%.6f,%.6f,%d,%s,%.1f,%.1f,%.1f",
            altitude, temperature, pressure,
            latitude, longitude, satellites, time.c_str(),
            pitch, roll, yaw);
    return std::string(buffer);
}
