#include <Wire.h>
#include "config.h"
#include "ina219.h"

#define INA219_REG_CONFIG        0x00
#define INA219_REG_SHUNT_VOLTAGE 0x01
#define INA219_REG_BUS_VOLTAGE   0x02
#define INA219_REG_POWER         0x03
#define INA219_REG_CURRENT       0x04
#define INA219_REG_CALIBRATION   0x05

#define CURRENT_LSB_A 0.0001f
#define INA219_CALIBRATION_VALUE 4096

static void ina219WriteRegister(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(INA219_ADDR);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

static int16_t ina219ReadRegister(uint8_t reg) {
  Wire.beginTransmission(INA219_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(INA219_ADDR, 2);
  if (Wire.available() < 2) return 0;
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  return (int16_t)((msb << 8) | lsb);
}

bool ina219Begin() {
  Wire.beginTransmission(INA219_ADDR);
  if (Wire.endTransmission() != 0) return false;

  uint16_t config = 0x399F;
  ina219WriteRegister(INA219_REG_CONFIG, config);
  delay(10);
  ina219WriteRegister(INA219_REG_CALIBRATION, INA219_CALIBRATION_VALUE);
  delay(10);
  return true;
}

bool ina219Read(INA219Data &data) {
  ina219WriteRegister(INA219_REG_CALIBRATION, INA219_CALIBRATION_VALUE);

  int16_t rawShunt = ina219ReadRegister(INA219_REG_SHUNT_VOLTAGE);
  int16_t rawBus   = ina219ReadRegister(INA219_REG_BUS_VOLTAGE);
  int16_t rawCurr  = ina219ReadRegister(INA219_REG_CURRENT);
  int16_t rawPower = ina219ReadRegister(INA219_REG_POWER);

  data.shuntVoltagemV = rawShunt * 0.01f;
  data.busVoltageV = ((rawBus >> 3) * 4.0f) / 1000.0f;
  data.currentmA = rawCurr * CURRENT_LSB_A * 1000.0f;
  data.powermW = rawPower * 20.0f * CURRENT_LSB_A * 1000.0f;
  data.ok = true;
  return true;
}
