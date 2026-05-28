#include <Wire.h>
#include "config.h"
#include "ds3231.h"

static uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

static uint8_t decToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

bool ds3231Read(DateTimeData &dt) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x00);

  if (Wire.endTransmission() != 0) {
    dt.ok = false;
    return false;
  }

  Wire.requestFrom(DS3231_ADDR, 7);
  if (Wire.available() != 7) {
    dt.ok = false;
    return false;
  }

  uint8_t rawSecond = Wire.read();
  uint8_t rawMinute = Wire.read();
  uint8_t rawHour   = Wire.read();
  Wire.read();
  uint8_t rawDay    = Wire.read();
  uint8_t rawMonth  = Wire.read();
  uint8_t rawYear   = Wire.read();

  dt.second = bcdToDec(rawSecond & 0x7F);
  dt.minute = bcdToDec(rawMinute & 0x7F);
  dt.hour   = bcdToDec(rawHour & 0x3F);
  dt.day    = bcdToDec(rawDay & 0x3F);
  dt.month  = bcdToDec(rawMonth & 0x1F);
  dt.year   = 2000 + bcdToDec(rawYear);
  dt.ok = true;
  return true;
}

void ds3231Adjust(int year, int month, int day, int hour, int minute, int second) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x00);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(1));
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year - 2000));
  Wire.endTransmission();
}

String formatTimestamp(const DateTimeData &dt) {
  if (!dt.ok) return "RTC_ERROR";
  char buffer[25];
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
  return String(buffer);
}
