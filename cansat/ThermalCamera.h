#ifndef THERMAL_CAMERA_H
#define THERMAL_CAMERA_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MLX90640.h>

// Holds the user-specified temperature detection range
struct TemperatureRange {
  float minTemp;
  float maxTemp;
};

// Represents coordinates of a pixel (x=0..31, y=0..23) on the MLX90640
struct PixelCoord {
  int x;
  int y;
};

class ThermalCamera {
public:
  // Constructor: default maxSpots=3, range=0..100
  ThermalCamera();

  // Initialize the MLX90640 with specific I2C pins and frequency
  bool begin(uint8_t sdaPin, uint8_t sclPin, uint32_t i2cFreq = 400000);

  // Set / get temperature range
  void setTemperatureRange(float minT, float maxT);
  TemperatureRange getTemperatureRange() const;

  // Set / get how many top spots we keep (maxSpots)
  void setMaxSpots(int m);
  int getMaxSpots() const;

  // Reads a frame, filters pixels within range,
  // and keeps the hottest maxSpots among them.
  // Returns how many hot spots it actually found (<= maxSpots).
  int detectHotSpots();

  // How many hot spots were found in the last detectHotSpots() call?
  int getFoundCount() const;

  // Get the i-th hot spot's coordinates (0..foundCount-1)
  PixelCoord getHotSpot(int i) const;

private:
  Adafruit_MLX90640 _mlx;     // MLX90640 sensor driver
  float _frameBuffer[768];    // Storage for the 24x32 pixel temperatures

  TemperatureRange _range;    // Current detection range (minTemp..maxTemp)
  int _maxSpots;              // Maximum number of hottest spots to store
  int _foundCount;            // How many spots we found last time

  // We store the top spots' coordinates in this array
  PixelCoord _foundSpots[768]; // In practice we only use up to _maxSpots.
};

#endif
