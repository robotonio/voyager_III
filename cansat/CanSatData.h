#ifndef CANSATDATA_H
#define CANSATDATA_H

#include <Arduino.h>
#include <vector>
#include <utility>

class CanSatData {
public:
  // Μέλη δεδομένων
  float altitude;       // σε μέτρα
  float temperature;    // σε °C
  float pressure;       // σε hPa
  String gps_time;      // σε "HH:MM:SS"
  float latitude;
  float longitude;
  float pitch;          // σε μοίρες
  float roll;           // σε μοίρες
  float yaw;            // σε μοίρες
  int is_vtx_on;        // 1 = True, 0 = False
  std::vector< std::pair<float, float> > hotspots; // ζεύγη (latitude, longitude)
  double timestamp;     // χρόνος λήψης δεδομένων (σε δευτερόλεπτα)

  // Constructors
  CanSatData();
  CanSatData(float altitude, float temperature, float pressure,
             const String &gps_time, float latitude, float longitude,
             float pitch, float roll, float yaw, int is_vtx_on,
             const std::vector< std::pair<float, float> > &hotspots, double timestamp);

  // Μετατρέπει τα δεδομένα σε string (μορφή CSV, περιτριγυρισμένο από #)
  String toString() const;

  // Στατική μέθοδος που προσπαθεί να αναλύσει ένα string και να γεμίσει ένα αντικείμενο CanSatData.
  // Επιστρέφει true αν η ανάλυση ήταν επιτυχής.
  static bool parseFromString(const String &data_str, CanSatData &data);

  // Ενημερώνει το αντικείμενο από ένα string
  bool updateFromString(const String &data_str);

  // Εκτυπώνει τα δεδομένα στο Serial Monitor
  void printData() const;

  // Στατική μέθοδος που δημιουργεί ένα dummy αντικείμενο με τυχαίες τιμές (dump data)
  static CanSatData createDump();
};

#endif
