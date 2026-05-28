#include <Wire.h>
#include "config.h"
#include "sht31.h"

bool sht31Read(SHT31Data &data) {
  Wire.beginTransmission(SHT31_ADDR);
  Wire.write(0x24); // single shot, high repeatability, no clock stretching
  Wire.write(0x00);

  if (Wire.endTransmission() != 0) {
    data.ok = false;
    return false;
  }

  delay(20);
  Wire.requestFrom(SHT31_ADDR, 6);

  if (Wire.available() != 6) {
    data.ok = false;
    return false;
  }

  uint8_t t_msb = Wire.read();
  uint8_t t_lsb = Wire.read();
  Wire.read(); // CRC temperatura no usado

  uint8_t h_msb = Wire.read();
  uint8_t h_lsb = Wire.read();
  Wire.read(); // CRC humedad no usado

  uint16_t rawTemp = ((uint16_t)t_msb << 8) | t_lsb;
  uint16_t rawHum  = ((uint16_t)h_msb << 8) | h_lsb;

  data.temperaturaC = -45.0f + 175.0f * ((float)rawTemp / 65535.0f);
  data.humedadRH = 100.0f * ((float)rawHum / 65535.0f);
  data.ok = true;
  return true;
}
