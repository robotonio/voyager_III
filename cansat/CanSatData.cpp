#include "CanSatData.h"

// Default constructor
CanSatData::CanSatData()
  : altitude(0.0), temperature(0.0), pressure(0.0), gps_time("00:00:00"),
    latitude(0.0), longitude(0.0), pitch(0.0), roll(0.0), yaw(0.0),
    is_vtx_on(0), timestamp(0.0) { }

// Parameterized constructor
CanSatData::CanSatData(float altitude, float temperature, float pressure,
                       const String &gps_time, float latitude, float longitude,
                       float pitch, float roll, float yaw, int is_vtx_on,
                       const std::vector< std::pair<float, float> > &hotspots, double timestamp)
  : altitude(altitude), temperature(temperature), pressure(pressure), gps_time(gps_time),
    latitude(latitude), longitude(longitude), pitch(pitch), roll(roll), yaw(yaw),
    is_vtx_on(is_vtx_on), hotspots(hotspots), timestamp(timestamp) { }

// toString: Μετατρέπει το αντικείμενο σε string μορφής:
// #altitude,temperature,pressure,gps_time,latitude,longitude,pitch,roll,yaw,is_vtx_on,hotspot1_lat,hotspot1_lng,...,timestamp#
String CanSatData::toString() const {
  String result = "#";
  result += String(altitude, 1) + ",";
  result += String(temperature, 1) + ",";
  result += String(pressure, 1) + ",";
  result += gps_time + ",";
  result += String(latitude, 4) + ",";
  result += String(longitude, 4) + ",";
  result += String(pitch, 1) + ",";
  result += String(roll, 1) + ",";
  result += String(yaw, 1) + ",";
  result += String(is_vtx_on);
  
  // Προσθήκη των hotspots, αν υπάρχουν
  for (size_t i = 0; i < hotspots.size(); i++) {
    result += "," + String(hotspots[i].first, 4) + "," + String(hotspots[i].second, 4);
  }
  // Προσθήκη timestamp στο τέλος
  result += "," + String(timestamp, 3);
  result += "#";
  return result;
}

// Βοηθητική συνάρτηση για "trim" (αφαίρεση κενού στις άκρες)
static String trim(const String &s) {
  int start = 0;
  while (start < s.length() && isspace(s.charAt(start))) {
    start++;
  }
  int end = s.length() - 1;
  while (end >= 0 && isspace(s.charAt(end))) {
    end--;
  }
  if (start > end) return "";
  return s.substring(start, end + 1);
}

// parseFromString: Αναλύει το string και γεμίζει το data.
// Το string πρέπει να ξεκινά και να τελειώνει με '#' και να περιέχει τουλάχιστον 10 πεδία.
bool CanSatData::parseFromString(const String &data_str, CanSatData &data) {
  String s = trim(data_str);
  if (!s.startsWith("#") || !s.endsWith("#")) {
    return false;
  }
  // Αφαιρούμε τα '#' στην αρχή και στο τέλος
  s = s.substring(1, s.length() - 1);
  s = trim(s);
  
  // Διαχωρίζουμε το string με βάση το κόμμα
  std::vector<String> parts;
  int startIdx = 0;
  int commaIdx = s.indexOf(',');
  while (commaIdx != -1) {
    parts.push_back(s.substring(startIdx, commaIdx));
    startIdx = commaIdx + 1;
    commaIdx = s.indexOf(',', startIdx);
  }
  parts.push_back(s.substring(startIdx));

  // Χρειάζονται τουλάχιστον 10 πεδία
  if (parts.size() < 10) {
    return false;
  }
  
  // Ανάλυση των βασικών πεδίων
  data.altitude = parts[0].toFloat();
  data.temperature = parts[1].toFloat();
  data.pressure = parts[2].toFloat();
  data.gps_time = trim(parts[3]);
  data.latitude = parts[4].toFloat();
  data.longitude = parts[5].toFloat();
  data.pitch = parts[6].toFloat();
  data.roll = parts[7].toFloat();
  data.yaw = parts[8].toFloat();
  data.is_vtx_on = parts[9].toInt();
  
  // Τα υπόλοιπα πεδία αφορούν τα hotspots και (ενδεχομένως) το timestamp
  size_t remaining = parts.size() - 10;
  std::vector<String> hotspotParts;
  for (size_t i = 10; i < parts.size(); i++) {
    hotspotParts.push_back(trim(parts[i]));
  }
  
  double timestampVal = 0.0;
  std::vector< std::pair<float, float> > hotspots;
  // Αν ο αριθμός των υπολοίπων πεδίων είναι περιττός, το τελευταίο είναι το timestamp
  if (remaining % 2 == 1 && remaining >= 1) {
    timestampVal = hotspotParts.back().toFloat();
    hotspotParts.pop_back();
  }
  data.timestamp = timestampVal;
  
  // Ανάλυση των ζευγών για τα hotspots
  for (size_t i = 0; i + 1 < hotspotParts.size(); i += 2) {
    float lat_hot = hotspotParts[i].toFloat();
    float lng_hot = hotspotParts[i + 1].toFloat();
    hotspots.push_back(std::make_pair(lat_hot, lng_hot));
  }
  data.hotspots = hotspots;
  
  return true;
}

// updateFromString: Ενημερώνει το αντικείμενο με τα δεδομένα από το string.
bool CanSatData::updateFromString(const String &data_str) {
  CanSatData newData;
  if (parseFromString(data_str, newData)) {
    *this = newData;
    return true;
  }
  return false;
}

// printData: Εκτυπώνει τα δεδομένα στο Serial Monitor.
void CanSatData::printData() const {
  Serial.println("=== CanSatData ===");
  Serial.print("Altitude:    "); Serial.println(altitude);
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Pressure:    "); Serial.println(pressure);
  Serial.print("GPS Time:    "); Serial.println(gps_time);
  Serial.print("Latitude:    "); Serial.println(latitude, 4);
  Serial.print("Longitude:   "); Serial.println(longitude, 4);
  Serial.print("Pitch:       "); Serial.println(pitch);
  Serial.print("Roll:        "); Serial.println(roll);
  Serial.print("Yaw:         "); Serial.println(yaw);
  Serial.print("isVTXOn:     "); Serial.println(is_vtx_on);
  Serial.print("Hotspots:    ");
  for (size_t i = 0; i < hotspots.size(); i++) {
    Serial.print("(");
    Serial.print(hotspots[i].first, 4);
    Serial.print(",");
    Serial.print(hotspots[i].second, 4);
    Serial.print(") ");
  }
  Serial.println();
  Serial.print("Timestamp:   "); Serial.println(timestamp, 3);
  Serial.println("==================");
}

// createDump: Δημιουργεί ένα dummy αντικείμενο με τυχαίες τιμές.
CanSatData CanSatData::createDump() {
  // Τυχαία δεδομένα σύμφωνα με τα όρια του Python παραδείγματος:
  float altitude = ((float) random(1000, 10000)) / 10.0;     // 100 - 1000 μέτρα, 1 δεκαδικό
  float temperature = ((float) random(200, 350)) / 10.0;         // 20 - 35 °C
  float pressure = ((float) random(9500, 10200)) / 10.0;         // 950 - 1020 hPa
  
  // Δημιουργία dummy gps_time σε μορφή HH:MM:SS
  int hour = random(0, 24);
  int minute = random(0, 60);
  int second = random(0, 60);
  char timeBuf[9];
  sprintf(timeBuf, "%02d:%02d:%02d", hour, minute, second);
  String gps_time(timeBuf);
  
  // Συντεταγμένες περιορισμένες στην περιοχή του Κορυδαλλού (περίπου 37.93-37.95 και 23.68-23.72)
  float latitude = ((float) random(379300, 379500)) / 10000.0;
  float longitude = ((float) random(236800, 237200)) / 10000.0;
  
  float pitch = ((float) random(0, 201)) / 10.0;  // 0 - 20°
  float roll  = ((float) random(0, 151)) / 10.0;  // 0 - 15°
  float yaw   = ((float) random(0, 101)) / 10.0;   // 0 - 10°
  int is_vtx_on = (random(0, 100) < 90) ? 1 : 0;
  
  int num_hotspots = random(1, 4);  // 1 έως 3 hotspots
  std::vector< std::pair<float, float> > hotspots;
  for (int i = 0; i < num_hotspots; i++) {
    // Ελαφρύ offset στο εύρος [-0.001, 0.001]
    float offsetLat = ((float) random(-1000, 1001)) / 1000000.0;
    float offsetLng = ((float) random(-1000, 1001)) / 1000000.0;
    // Στρογγυλοποίηση στα 4 δεκαδικά
    float h_lat = round((latitude + offsetLat) * 10000) / 10000.0;
    float h_lng = round((longitude + offsetLng) * 10000) / 10000.0;
    hotspots.push_back(std::make_pair(h_lat, h_lng));
  }
  
  // Χρήση millis() ως dummy timestamp (σε δευτερόλεπτα)
  double timestamp = millis() / 1000.0;
  
  return CanSatData(altitude, temperature, pressure, gps_time, latitude, longitude,
                    pitch, roll, yaw, is_vtx_on, hotspots, timestamp);
}
