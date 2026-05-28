#pragma once
#include <Arduino.h>
#include "data_types.h"

bool oledBegin();
void oledShowINA219(const INA219Data &ina);
void oledShowFull(const SHT31Data &sht, const INA219Data &ina, const ADS1115Data &ads);
void oledShowError(const char* message);
