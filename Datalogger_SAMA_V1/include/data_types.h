#pragma once
#include <Arduino.h>

struct DateTimeData {
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  bool ok = false;
};

struct SHT31Data {
  float temperaturaC = 0.0f;
  float humedadRH = 0.0f;
  bool ok = false;
};

struct ADS1115Data {
  float voltajeA0 = 0.0f;
  float corrienteA0_mA = 0.0f;
  float porcentaje4_20 = 0.0f;
  float voltajeA1 = 0.0f;
  float voltajeA2 = 0.0f;
  float voltajeA3 = 0.0f;
  bool ok = false;
};

struct INA219Data {
  float busVoltageV = 0.0f;
  float shuntVoltagemV = 0.0f;
  float currentmA = 0.0f;
  float powermW = 0.0f;
  bool ok = false;
};

struct NanoSDI12Data {
  float waterPotentialKPa = 0.0f;
  float temperaturaC = 0.0f;
  bool available = false;
  bool jsonOk = false;
  String raw = "";
};
