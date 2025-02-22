#include "ThermalCamera.h"

ThermalCamera::ThermalCamera(bool debug)
: _maxSpots(3),
  _foundCount(0),
  debug(debug)
{
  // Default range 0..100 Celsius
  _range.minTemp = 0.0f;
  _range.maxTemp = 100.0f;
}

bool ThermalCamera::begin(uint8_t sdaPin, uint8_t sclPin, uint32_t i2cFreq) {
  Wire.begin(sdaPin, sclPin, i2cFreq);

  // Try to initialize at default I2C address (0x33)
  if (!_mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    return false; // Not detected
  }
  // Some default MLX90640 settings
  _mlx.setMode(MLX90640_CHESS);
  _mlx.setResolution(MLX90640_ADC_18BIT);
  _mlx.setRefreshRate(MLX90640_4_HZ);
  randomSeed(analogRead(A0));

  return true;
}

void ThermalCamera::setTemperatureRange(float minT, float maxT) {
  _range.minTemp = minT;
  _range.maxTemp = maxT;
}

TemperatureRange ThermalCamera::getTemperatureRange() const {
  return _range;
}

void ThermalCamera::setMaxSpots(int m) {
  // Prevent invalid or zero
  if (m < 1) m = 1;
  _maxSpots = m;
}

int ThermalCamera::getMaxSpots() const {
  return _maxSpots;
}

int ThermalCamera::detectHotSpots() {
  if (!debug){
    _foundCount = 0; // reset

    // Try reading a frame (returns 0 on success)
    if (_mlx.getFrame(_frameBuffer) != 0) {
      // If failed, we keep 0 found
      return 0;
    }

    // We'll define a small structure to keep track of top spots
    struct PixelData {
      float temp;
      int x;
      int y;
    };

    // A local array to store up to _maxSpots hottest pixels
    // (far smaller than storing all 768).
    PixelData best[32]; 
    // Why 32? => If your maxSpots can be bigger, consider best[_maxSpots].
    // But to be safe if user sets maxSpots = 50, you'd need a bigger array or dynamic allocation.
    // For demonstration let's assume _maxSpots <= 32. You can increase if needed.

    int bestCount = 0; // how many we have stored so far

    // 1) Iterate all 768 pixels
    for (int i = 0; i < 768; i++) {
      float t = _frameBuffer[i];
      if (t >= _range.minTemp && t <= _range.maxTemp) {
        // Convert index to (row, col)
        int row = i / 32; // 0..23
        int col = i % 32; // 0..31

        // If we have space in 'best'
        if (bestCount < _maxSpots && bestCount < 32) {
          best[bestCount].temp = t;
          best[bestCount].x = col;
          best[bestCount].y = row;
          bestCount++;
        } else {
          // The array is full => check if this pixel is hotter than the coldest in best
          // Find the minimum temp inside 'best'
          float minVal = best[0].temp;
          int minIndex = 0;
          for (int j = 1; j < bestCount; j++) {
            if (best[j].temp < minVal) {
              minVal = best[j].temp;
              minIndex = j;
            }
          }
          // If new pixel is hotter than the min in 'best', replace it
          if (t > minVal) {
            best[minIndex].temp = t;
            best[minIndex].x = col;
            best[minIndex].y = row;
          }
        }
      }
    }

    _foundCount = bestCount; // up to maxSpots

    // 2) Sort the 'best' array in descending order by temp (simple bubble sort)
    // We'll only sort the first _foundCount elements
    for (int i = 0; i < _foundCount - 1; i++) {
      for (int j = 0; j < _foundCount - i - 1; j++) {
        if (best[j].temp < best[j + 1].temp) {
          // swap
          PixelData tmp = best[j];
          best[j] = best[j + 1];
          best[j + 1] = tmp;
        }
      }
    }

    // 3) Copy them into our _foundSpots array
    // (the class stores coords, not temps)
    for (int i = 0; i < _foundCount; i++) {
      _foundSpots[i].x = best[i].x;
      _foundSpots[i].y = best[i].y;
    }

    return _foundCount;
  }
  else { // debug == true
    for (int i = 0; i < _foundCount; i++) {
      _foundSpots[i].x = random(0, 60);
      _foundSpots[i].y = random(0, 60);
    }
    _foundCount = random(0, _maxSpots);
    return _foundCount;
  }
}

int ThermalCamera::getFoundCount() const {
  return _foundCount;
}

PixelCoord ThermalCamera::getHotSpot(int i) const {
  if (i < 0 || i >= _foundCount) {
    // Return invalid coords if out of range
    PixelCoord invalid = { -1, -1 };
    return invalid;
  }
  return _foundSpots[i];
}
