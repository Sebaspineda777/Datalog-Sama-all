#include <Wire.h>
#include "config.h"
#include "ads1115.h"

#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG     0x01
#define ADS1115_LSB_4_096      0.000125f

static void ads1115WriteRegister(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(ADS1115_ADDR);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

static int16_t ads1115ReadRegister(uint8_t reg) {
  Wire.beginTransmission(ADS1115_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(ADS1115_ADDR, 2);
  if (Wire.available() < 2) return 0;
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  return (int16_t)((msb << 8) | lsb);
}

bool ads1115Begin() {
  Wire.beginTransmission(ADS1115_ADDR);
  return Wire.endTransmission() == 0;
}

bool ads1115ReadChannel(uint8_t canal, float &voltaje) {
  if (canal > 3) return false;

  uint16_t mux = 0;
  switch (canal) {
    case 0: mux = 0x4000; break;
    case 1: mux = 0x5000; break;
    case 2: mux = 0x6000; break;
    case 3: mux = 0x7000; break;
  }

  uint16_t config = 0;
  config |= 0x8000; // start conversion
  config |= mux;
  config |= 0x0200; // PGA +/-4.096 V
  config |= 0x0100; // single-shot
  config |= 0x0080; // 128 SPS
  config |= 0x0003; // comparator disabled

  ads1115WriteRegister(ADS1115_REG_CONFIG, config);
  delay(10);

  int16_t raw = ads1115ReadRegister(ADS1115_REG_CONVERSION);
  if (raw < 0) raw = 0;
  voltaje = raw * ADS1115_LSB_4_096;
  return true;
}

bool ads1115ReadAll(ADS1115Data &data) {
  bool ok0 = ads1115ReadChannel(0, data.voltajeA0);
  bool ok1 = ads1115ReadChannel(1, data.voltajeA1);
  bool ok2 = ads1115ReadChannel(2, data.voltajeA2);
  bool ok3 = ads1115ReadChannel(3, data.voltajeA3);

  data.corrienteA0_mA = (data.voltajeA0 / R_SHUNT_OHMS) * 1000.0f;
  data.porcentaje4_20 = ((data.corrienteA0_mA - 4.0f) / 16.0f) * 100.0f;
  if (data.porcentaje4_20 < 0) data.porcentaje4_20 = 0;
  if (data.porcentaje4_20 > 100) data.porcentaje4_20 = 100;

  data.ok = ok0 && ok1 && ok2 && ok3;
  return data.ok;
}
