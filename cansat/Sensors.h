#ifndef SENSORS_H
#define SENSORS_H

#include <string> // Για std::string
#include "BMP280.h"
#include "IMU.h"
#include "GPSModule.h"

class Sensors {
  private:
    BMP280 bmp280;          // Αντικείμενο BMP280
    IMU imu;                // Αντικείμενο IMU
    GPSModule gpsModule;    // Αντικείμενο GPSModule

    // Ιδιότητες για κάθε τιμή που διαβάζεται
    float altitude;         // Υψόμετρο (BMP280)
    float temperature;      // Θερμοκρασία (BMP280)
    float pressure;         // Πίεση (BMP280)

    float latitude;         // Γεωγραφικό πλάτος (GPS)
    float longitude;        // Γεωγραφικό μήκος (GPS)
    int satellites;         // Πλήθος δορυφόρων (GPS)
    String time;            // Ώρα UTC (GPS)

    float pitch;            // Γωνία Pitch (IMU)
    float roll;             // Γωνία Roll (IMU)
    float yaw;              // Γωνία Yaw (IMU)

    bool bmpInitialized;    // Κατάσταση BMP280
    bool imuInitialized;    // Κατάσταση IMU
    bool gpsInitialized;    // Κατάσταση GPS

  public:
    // Constructor
    Sensors();

    // Αρχικοποίηση αισθητήρων
    void initialize();

    // Μέθοδος για ανάγνωση όλων των τιμών
    void read();

    // Getters για κάθε τιμή
    float getAltitude() const;
    float getTemperature() const;
    float getPressure() const;

    float getLatitude() const;
    float getLongitude() const;
    int getSatellites() const;
    String getTime() const;

    float getPitch() const;
    float getRoll() const;
    float getYaw() const;

    // Μέθοδος to_string για όλες τις τιμές
    std::string to_string() const;
};

#endif // SENSORS_H
